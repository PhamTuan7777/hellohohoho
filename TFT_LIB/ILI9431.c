#include <stdlib.h> // malloc()
#include <string.h> // memset()
#include <ctype.h>

//#include "ILI9431.h"
#include "ILI9431_gfx.h"
//#include "ILI9431_font.h"

// ---------------------------------------------------------- private defines --

#define __ILI9341_TOUCH_NORM_SAMPLES__ 8U

// ----------------------------------------------------------- private macros --

#define __IS_SPI_SLAVE(s) (((s) > issNONE) && ((s) < issCOUNT))

#define __SLAVE_SELECT(d, s)  \
  if (__IS_SPI_SLAVE(s)) { ili9341_spi_slave_select((d), (s)); }

#define __SLAVE_RELEASE(d, s) \
  if (__IS_SPI_SLAVE(s)) { ili9341_spi_slave_release((d), (s)); }


// Registers
static const uint8_t REG_DEVID = 0x00;
static const uint8_t REG_POWER_CTL = 0x2D;
static const uint8_t REG_DATAX0 = 0x32;

// Other constants
static const uint8_t DEVID = 0xE5;
static const float SENSITIVITY_2G = 1.0 / 256;  // (g/LSB)
static const float EARTH_GRAVITY = 9.80665; 
// ------------------------------------------------------------ private types --

/* nothing */

// ------------------------------------------------------- exported variables --

/* nothing */

// -------------------------------------------------------- private variables --

/* nothing */

// ---------------------------------------------- private function prototypes --

static void ili9341_reset(ili9341_t *lcd);
static void ili9341_initialize(ili9341_t *lcd);
static ili9341_two_dimension_t ili9341_screen_size(
    ili9341_screen_orientation_t orientation);
static uint8_t ili9341_screen_rotation(
    ili9341_screen_orientation_t orientation);

static int32_t interp(int32_t x, int32_t x0, int32_t x1, int32_t y0, int32_t y1);
ili9341_two_dimension_t ili9341_clip_touch_coordinate(ili9341_two_dimension_t coord,
    ili9341_two_dimension_t min, ili9341_two_dimension_t max);
ili9341_two_dimension_t ili9341_project_touch_coordinate(ili9341_t *lcd,
    uint16_t x_pos, uint16_t y_pos);


// ------------------------------------------------------- exported functions --

ili9341_t *ili9341_new(
    spi_inst_t *spi, 
    uint sck_pin, uint mosi_pin, uint miso_pin,
    uint tft_cs_pin, uint reset_pin, uint data_cmd_pin,

    ili9341_screen_orientation_t orientation,

    uint touch_cs_pin, uint touch_irq_pin,

    ili9341_touch_support_t   touch_support,
    ili9341_touch_normalize_t touch_normalize
)
{
  ili9341_t *lcd = NULL;
  if (NULL != spi) {
    if ((orientation > isoNONE) && (orientation < isoCOUNT) &&
        (NULL != data_cmd_pin) && (NULL != tft_cs_pin) &&
        (NULL != tft_cs_pin))
    {
      if ( itsSupported != touch_support || ( (NULL != touch_cs_pin) && (NULL != touch_irq_pin)))
      {
        if (NULL != (lcd = malloc(sizeof(ili9341_t))))
        {
          lcd->spi = spi;

          lcd->reset_pin = reset_pin;
          lcd->tft_cs_pin = tft_cs_pin;
          lcd->data_cmd_pin = data_cmd_pin;

          lcd->orientation = orientation;
          lcd->screen_size = ili9341_screen_size(orientation);

          if (touch_support)
          {
            lcd->touch_cs_pin = touch_cs_pin;
            lcd->touch_irq_pin = touch_irq_pin;

            lcd->touch_support = touch_support;
            lcd->touch_normalize = touch_normalize;
            lcd->touch_coordinate = (ili9341_two_dimension_t){ {0U}, {0U} };
            lcd->touch_calibration = itcNONE;
            lcd->touch_scalar = (ili9341_scalar_calibrator_t){ {{0U}, {0U}}, {{0U}, {0U}} };
            lcd->touch_3point = (ili9341_3point_calibrator_t){ {{0U}, {0U}}, 0, 0, 0.0F, 0.0F, 0.0F, 0.0F };

            lcd->touch_pressed        = itpNotPressed;
            lcd->touch_pressed_begin  = NULL;
            lcd->touch_pressed_end    = NULL;
          }
          else
          {
            lcd->touch_cs_pin = 100;
            lcd->touch_irq_pin = 100;

            lcd->touch_support = touch_support;
            lcd->touch_normalize = itpNONE;
            lcd->touch_coordinate = (ili9341_two_dimension_t){ {0U}, {0U} };
            lcd->touch_calibration    = itcNONE;
            lcd->touch_scalar         = (ili9341_scalar_calibrator_t){ {{0U}, {0U}}, {{0U}, {0U}} };
            lcd->touch_3point         = (ili9341_3point_calibrator_t){ {{0U}, {0U}}, 0, 0, 0.0F, 0.0F, 0.0F, 0.0F };

            lcd->touch_pressed        = itpNONE;
            lcd->touch_pressed_begin  = NULL;
            lcd->touch_pressed_end    = NULL;
          }

          ili9341_initialize(lcd);
        }
      }
    }
  }
}

void ili9341_touch_interrupt(ili9341_t *lcd)
{
  uint16_t x_pos;
  uint16_t y_pos;

  // read the new/incoming state of the touch screen
  ili9341_touch_pressed_t pressed =
      ili9341_touch_coordinate(lcd, &x_pos, &y_pos);

  // switch path based on existing/prior state of the touch screen. note this
  // requires the touch interrupt GPIO EXTI be set to detect both falling and
  // rising edges.
  switch (lcd->touch_pressed) {
    case itpNONE:
    case itpNotPressed:
      if (itpPressed == pressed) {
        // state change, start of press
        if (NULL != lcd->touch_pressed_begin) {
          // use the current, normalized touch coordinate
          lcd->touch_pressed_begin(lcd, x_pos, y_pos);
        }
      }
      break;

    case itpPressed:
      if ((itpNONE == pressed) || (itpNotPressed == pressed)) {
        // state change, end of press
        if (NULL != lcd->touch_pressed_end) {
          // use the last-known valid touch coordinate, since the current
          // state does not have a valid touch.
          lcd->touch_pressed_end(lcd,
              lcd->touch_coordinate.x, lcd->touch_coordinate.y);
        }
      }
      break;

    default:
      break;
  }

  // update the internal state with current state of touch screen
  if (pressed != lcd->touch_pressed) {
    lcd->touch_pressed = pressed;
  }

  if (itpPressed == pressed) {
    lcd->touch_coordinate.x = x_pos;
    lcd->touch_coordinate.y = y_pos;
  }
}

ili9341_touch_pressed_t ili9341_touch_pressed(ili9341_t *lcd)
{
  if (NULL == lcd)
    { return itpNONE; }

  if (gpio_get(lcd->touch_irq_pin) == 1)
    { return itpPressed; }
  else
    { return itpNotPressed; }
}

void ili9341_set_touch_pressed_begin(ili9341_t *lcd, ili9341_touch_callback_t callback)
{
  if ((NULL != lcd) && (NULL != callback)) {
    lcd->touch_pressed_begin = callback;
  }
}

void ili9341_set_touch_pressed_end(ili9341_t *lcd, ili9341_touch_callback_t callback)
{
  if ((NULL != lcd) && (NULL != callback)) {
    lcd->touch_pressed_end = callback;
  }
}

ili9341_touch_pressed_t ili9341_touch_coordinate(ili9341_t *lcd,
                                                 uint16_t *x_pos, uint16_t *y_pos)
{
  if(lcd == NULL)
  {
    return itpNONE;
  }
  uint16_t req_samples;
  switch (lcd->touch_normalize) {
    default:
    case itnNotNormalized:
      req_samples = 1;
      break;
    case itnNormalized:
      req_samples = __ILI9341_TOUCH_NORM_SAMPLES__;
      break;
  }
  static uint8_t x_cmd[]  = { 0xD3 };
  static uint8_t y_cmd[]  = { 0x93 };
  static uint8_t sleep[]  = { 0x00 };

  uint32_t x_avg = 0U;
  uint32_t y_avg = 0U;

  uint16_t sample = req_samples;
  uint16_t num_samples = 0U;

  spi_init(lcd->spi, 1000 * 1000);
  
  ili9341_spi_touch_select(lcd);

  while ((itpPressed == ili9341_touch_pressed(lcd)) && (sample--)) {

    uint8_t x_raw[2];
    uint8_t y_raw[2];

    spi_write_blocking(lcd->spi, x_cmd, sizeof(x_cmd));
    spi_read_blocking(lcd->spi, 0,  x_cmd, sizeof(x_cmd));

    spi_write_blocking(lcd->spi, y_cmd, sizeof(y_cmd));
    spi_read_blocking(lcd->spi, 0, y_cmd, sizeof(y_cmd));

    x_avg += __LEu16(x_raw) >> 3;
    y_avg += __LEu16(y_raw) >> 3;

    ++num_samples;
  }
  spi_write_blocking(lcd->spi, sleep, sizeof(sleep));

  ili9341_spi_touch_release(lcd);

  if (num_samples < req_samples)
    { return itpNotPressed; }

  ili9341_two_dimension_t coord =
      ili9341_project_touch_coordinate(lcd, x_avg / req_samples, y_avg / req_samples);

  *x_pos = coord.x;
  *y_pos = coord.y;

  return itpPressed;
}

void ili9341_calibrate_scalar(ili9341_t *lcd,
    uint16_t min_x, uint16_t min_y, uint16_t max_x, uint16_t max_y)
{
  if (NULL == lcd)
    { return; }

  lcd->touch_calibration = itcScalar;
  lcd->touch_scalar.min = (ili9341_two_dimension_t){ {min_x}, {min_y} };
  lcd->touch_scalar.max = (ili9341_two_dimension_t){ {max_x}, {max_y} };
}

void ili9341_calibrate_3point(ili9341_t *lcd,
    uint16_t scale_width, uint16_t scale_height,
    int32_t screen_a_x, int32_t screen_a_y,
    int32_t screen_b_x, int32_t screen_b_y,
    int32_t screen_c_x, int32_t screen_c_y,
    int32_t touch_a_x,  int32_t touch_a_y,
    int32_t touch_b_x,  int32_t touch_b_y,
    int32_t touch_c_x,  int32_t touch_c_y)
{
  if (NULL == lcd)
    { return; }

  lcd->touch_calibration = itc3Point;

  lcd->touch_3point.scale = (ili9341_two_dimension_t){ {scale_width}, {scale_height} };

  int32_t delta =
      ( (touch_a_x - touch_c_x) * (touch_b_y - touch_c_y) )
      -
      ( (touch_b_x - touch_c_x) * (touch_a_y - touch_c_y) );

  lcd->touch_3point.alpha_x =
      (float)
        ( ( (screen_a_x - screen_c_x) * (touch_b_y - touch_c_y) )
          -
          ( (screen_b_x - screen_c_x) * (touch_a_y - touch_c_y) ) )
      / delta;

  lcd->touch_3point.beta_x =
      (float)
        ( ( (touch_a_x - touch_c_x) * (screen_b_x - screen_c_x) )
          -
          ( (touch_b_x - touch_c_x) * (screen_a_x - screen_c_x) ) )
      / delta;

  lcd->touch_3point.delta_x =
      (float)(
        ( ( (int64_t)screen_a_x
              * ( (touch_b_x * touch_c_y) - (touch_c_x * touch_b_y) ) )
          -
          ( (int64_t)screen_b_x
              * ( (touch_a_x * touch_c_y) - (touch_c_x * touch_a_y) ) )
          +
          ( (int64_t)screen_c_x
              * ( (touch_a_x * touch_b_y) - (touch_b_x * touch_a_y) ) ) )
      ) / delta + 0.5;

  lcd->touch_3point.alpha_y =
      (float)
        ( ( (screen_a_y - screen_c_y) * (touch_b_y - touch_c_y) )
          -
          ( (screen_b_y - screen_c_y) * (touch_a_y - touch_c_y) ) )
      / delta;

  lcd->touch_3point.beta_y =
      (float)
        ( ( (touch_a_x - touch_c_x) * (screen_b_y - screen_c_y) )
          -
          ( (touch_b_x - touch_c_x) * (screen_a_y - screen_c_y) ) )
      / delta;

  lcd->touch_3point.delta_y =
      (float)(
        ( ( (int64_t)screen_a_y
              * (touch_b_x * touch_c_y - touch_c_x * touch_b_y) )
          -
          ( (int64_t)screen_b_y
              * (touch_a_x * touch_c_y - touch_c_x * touch_a_y) )
          +
          ( (int64_t)screen_c_y
              * (touch_a_x * touch_b_y - touch_b_x * touch_a_y) ) )
      ) / delta + 0.5;
}

void ili9341_spi_tft_select(ili9341_t *lcd)
{
  gpio_put(lcd->tft_cs_pin, 0);
}

void ili9341_spi_tft_release(ili9341_t *lcd)
{
  gpio_put(lcd->tft_cs_pin, 1);
}

void ili9341_spi_touch_select(ili9341_t *lcd)
{
  // clear bit indicates the touch screen is -active- slave SPI device
  gpio_put(lcd->touch_cs_pin,0);
}

void ili9341_spi_touch_release(ili9341_t *lcd)
{
  // set bit indicates the touch screen is -inactive- slave SPI device
  gpio_put(lcd->touch_cs_pin,1);
}

void ili9341_spi_slave_select(ili9341_t *lcd,
    ili9341_spi_slave_t spi_slave)
{
  switch (spi_slave) {
    case issDisplayTFT:  ili9341_spi_tft_select(lcd);   break;
    case issTouchScreen: ili9341_spi_touch_select(lcd); break;
    default: break;
  }
}

void ili9341_spi_slave_release(ili9341_t *lcd,
    ili9341_spi_slave_t spi_slave)
{
  switch (spi_slave) {

    case issDisplayTFT:  ili9341_spi_tft_release(lcd);   break;
    case issTouchScreen: ili9341_spi_touch_release(lcd); break;
    default: break;
  }
}

void ili9341_spi_write_command(ili9341_t *lcd,
    ili9341_spi_slave_t spi_slave, uint8_t command)
{
  __SLAVE_SELECT(lcd, spi_slave);

  gpio_put(lcd->data_cmd_pin,0);
  spi_write_blocking(lcd->spi, &command, sizeof(command));

  __SLAVE_RELEASE(lcd, spi_slave);
}

void ili9341_spi_write_data(ili9341_t *lcd,
    ili9341_spi_slave_t spi_slave, uint16_t data_sz, uint8_t data[])
{
  __SLAVE_SELECT(lcd, spi_slave);

  gpio_put(lcd->data_cmd_pin,0);
  spi_write_blocking(lcd->spi, data, data_sz);

  __SLAVE_RELEASE(lcd, spi_slave);
}

void ili9341_spi_write_data_read(ili9341_t *lcd,
    ili9341_spi_slave_t spi_slave,
    uint16_t data_sz, uint8_t tx_data[], uint8_t rx_data[])
{
  __SLAVE_SELECT(lcd, spi_slave);

  gpio_put(lcd->data_cmd_pin,1);
  spi_write_read_blocking(lcd->spi, tx_data, rx_data, data_sz);

  __SLAVE_RELEASE(lcd, spi_slave);
}

void ili9341_spi_write_command_data(ili9341_t *lcd,
    ili9341_spi_slave_t spi_slave, uint8_t command, uint16_t data_sz, uint8_t data[])
{
  __SLAVE_SELECT(lcd, spi_slave);

  ili9341_spi_write_command(lcd, issNONE, command);
  ili9341_spi_write_data(lcd, issNONE, data_sz, data);

  __SLAVE_RELEASE(lcd, spi_slave);  
}

static void ili9341_reset(ili9341_t *lcd)
{
  // the reset pin on ILI9341 is active low, so driving low temporarily will
  // reset the device (also resets the touch screen peripheral)
  gpio_put(lcd->reset_pin,0);
  sleep_ms(200);
  gpio_put(lcd->reset_pin,1);

  // ensure both slave lines are open
  ili9341_spi_tft_release(lcd);
  ili9341_spi_touch_release(lcd);
}

static void ili9341_initialize(ili9341_t *lcd)
{
  ili9341_reset(lcd);
  ili9341_spi_tft_select(lcd);

  // command list is based on https://github.com/martnak/STM32-ILI9341

  // SOFTWARE RESET
  ili9341_spi_write_command(lcd, issNONE, 0x01);
  sleep_ms(1000);

  // POWER CONTROL A
  ili9341_spi_write_command_data(lcd, issNONE,
      0xCB, 5, (uint8_t[]){ 0x39, 0x2C, 0x00, 0x34, 0x02 });

  // POWER CONTROL B
  ili9341_spi_write_command_data(lcd, issNONE,
      0xCF, 3, (uint8_t[]){ 0x00, 0xC1, 0x30 });

  // DRIVER TIMING CONTROL A
  ili9341_spi_write_command_data(lcd, issNONE,
      0xE8, 3, (uint8_t[]){ 0x85, 0x00, 0x78 });

  // DRIVER TIMING CONTROL B
  ili9341_spi_write_command_data(lcd, issNONE,
      0xEA, 2, (uint8_t[]){ 0x00, 0x00 });

  // POWER ON SEQUENCE CONTROL
  ili9341_spi_write_command_data(lcd, issNONE,
      0xED, 4, (uint8_t[]){ 0x64, 0x03, 0x12, 0x81 });

  // PUMP RATIO CONTROL
  ili9341_spi_write_command_data(lcd, issNONE,
      0xF7, 1, (uint8_t[]){ 0x20 });

  // POWER CONTROL,VRH[5:0]
  ili9341_spi_write_command_data(lcd, issNONE,
      0xC0, 1, (uint8_t[]){ 0x23 });

  // POWER CONTROL,SAP[2:0];BT[3:0]
  ili9341_spi_write_command_data(lcd, issNONE,
      0xC1, 1, (uint8_t[]){ 0x10 });

  // VCM CONTROL
  ili9341_spi_write_command_data(lcd, issNONE,
      0xC5, 2, (uint8_t[]){ 0x3E, 0x28 });

  // VCM CONTROL 2
  ili9341_spi_write_command_data(lcd, issNONE,
      0xC7, 1, (uint8_t[]){ 0x86 });

  // MEMORY ACCESS CONTROL
  ili9341_spi_write_command_data(lcd, issNONE,
      0x36, 1, (uint8_t[]){ 0x48 });

  // PIXEL FORMAT
  ili9341_spi_write_command_data(lcd, issNONE,
      0x3A, 1, (uint8_t[]){ 0x55 });

  // FRAME RATIO CONTROL, STANDARD RGB COLOR
  ili9341_spi_write_command_data(lcd, issNONE,
      0xB1, 2, (uint8_t[]){ 0x00, 0x18 });

  // DISPLAY FUNCTION CONTROL
  ili9341_spi_write_command_data(lcd, issNONE,
      0xB6, 3, (uint8_t[]){ 0x08, 0x82, 0x27 });

  // 3GAMMA FUNCTION DISABLE
  ili9341_spi_write_command_data(lcd, issNONE,
      0xF2, 1, (uint8_t[]){ 0x00 });

  // GAMMA CURVE SELECTED
  ili9341_spi_write_command_data(lcd, issNONE,
      0x26, 1, (uint8_t[]){ 0x01 });

  // POSITIVE GAMMA CORRECTION
  ili9341_spi_write_command_data(lcd, issNONE,
      0xE0, 15, (uint8_t[]){ 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
                             0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00 });

  // NEGATIVE GAMMA CORRECTION
  ili9341_spi_write_command_data(lcd, issNONE,
      0xE1, 15, (uint8_t[]){ 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
                             0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F });

  // EXIT SLEEP
  ili9341_spi_write_command(lcd, issNONE, 0x11);
  sleep_ms(120);

  // TURN ON DISPLAY
  ili9341_spi_write_command(lcd, issNONE, 0x29);

  // MADCTL
  ili9341_spi_write_command_data(lcd, issNONE,
      0x36, 1, (uint8_t[]){ ili9341_screen_rotation(lcd->orientation) });

  ili9341_spi_tft_release(lcd);
}

static ili9341_two_dimension_t ili9341_screen_size(
    ili9341_screen_orientation_t orientation)
{
  switch (orientation) {
    default:
    case isoDown:
      return (ili9341_two_dimension_t){ { .width = 240U }, { .height = 320U } };
    case isoRight:
      return (ili9341_two_dimension_t){ { .width = 320U }, { .height = 240U } };
    case isoUp:
      return (ili9341_two_dimension_t){ { .width = 240U }, { .height = 320U } };
    case isoLeft:
      return (ili9341_two_dimension_t){ { .width = 320U }, { .height = 240U } };
  }
}

static uint8_t ili9341_screen_rotation(
    ili9341_screen_orientation_t orientation)
{
  switch (orientation) {
    default:
    case isoDown:
      return 0x40 | 0x08;
    case isoRight:
      return 0x40 | 0x80 | 0x20 | 0x08;
    case isoUp:
      return 0x80 | 0x08;
    case isoLeft:
      return 0x20 | 0x08;
  }
}

static int32_t interp(int32_t x, int32_t x0, int32_t x1, int32_t y0, int32_t y1)
{
  if (x1 == x0)
    { return 0; } // return 0 on divide-by-zero

  return (x - x0) * (y1 - y0) / (x1 - x0) + y0;
}

ili9341_two_dimension_t ili9341_clip_touch_coordinate(ili9341_two_dimension_t coord,
    ili9341_two_dimension_t min, ili9341_two_dimension_t max)
{
  if (coord.x < min.x) { coord.x = min.x; }
  if (coord.x > max.x) { coord.x = max.x; }
  if (coord.y < min.y) { coord.y = min.y; }
  if (coord.y > max.y) { coord.y = max.y; }

  return coord;
}

ili9341_two_dimension_t ili9341_project_touch_coordinate(ili9341_t *lcd,
    uint16_t x_pos, uint16_t y_pos)
{
  ili9341_two_dimension_t coord = (ili9341_two_dimension_t){ {x_pos}, {y_pos} };
  ili9341_two_dimension_t rotate;
  int32_t x_scaled, y_scaled;

  if (NULL != lcd) {

    switch (lcd->touch_calibration) {
      case itcScalar:

        x_scaled = coord.x; // otherwise we get "may be used uninitialized" warning even
        y_scaled = coord.y; // though all switch() cases are handled

        switch (lcd->orientation % isoCOUNT) {
          case isoPortrait:
          case isoPortraitFlip:
            x_scaled = interp(coord.x,
                lcd->touch_scalar.min.x, lcd->touch_scalar.max.x,
                0U, lcd->screen_size.width);
            y_scaled = interp(coord.y,
                lcd->touch_scalar.min.y, lcd->touch_scalar.max.y,
                0U, lcd->screen_size.height);
            break;

          case isoLandscape:
            x_scaled = interp(coord.y,
                lcd->touch_scalar.min.y, lcd->touch_scalar.max.y,
                0U, lcd->screen_size.width);
            y_scaled = interp(coord.x,
                lcd->touch_scalar.min.x, lcd->touch_scalar.max.x,
                0U, lcd->screen_size.height);
            break;

          case isoLandscapeFlip:
            x_scaled = interp(coord.y,
                lcd->touch_scalar.min.y, lcd->touch_scalar.max.y,
                0U, lcd->screen_size.width);
            y_scaled = interp(coord.x,
                lcd->touch_scalar.min.x, lcd->touch_scalar.max.x,
                0U, lcd->screen_size.height);
            break;
        }

        coord = ili9341_clip_touch_coordinate(
            (ili9341_two_dimension_t){{x_scaled}, {y_scaled}},
            (ili9341_two_dimension_t){{0U}, {0U}},
            (ili9341_two_dimension_t){{lcd->screen_size.width}, {lcd->screen_size.height}});

        break;

      case itc3Point:

        coord = (ili9341_two_dimension_t){
          .x = __FROUND(uint16_t, lcd->touch_3point.alpha_x * coord.x +
            lcd->touch_3point.beta_x * coord.y +
            lcd->touch_3point.delta_x
          ),
          .y = __FROUND(uint16_t, lcd->touch_3point.alpha_y * coord.x +
            lcd->touch_3point.beta_y * coord.y +
            lcd->touch_3point.delta_y
          )
        };

        coord = ili9341_clip_touch_coordinate(
            rotate,
            (ili9341_two_dimension_t){ {0U}, {0U} },
            lcd->touch_3point.scale);

        switch (lcd->orientation % isoCOUNT) {

          case isoPortrait:
            rotate = (ili9341_two_dimension_t){
                .x = lcd->touch_3point.scale.width - coord.y,
                .y = coord.x
            };
            break;

          case isoLandscape:
            rotate = (ili9341_two_dimension_t){
              .x = coord.x,
              .y = coord.y
            };
            break;

          case isoPortraitFlip:
            rotate = (ili9341_two_dimension_t){
              .x = coord.y,
              .y = lcd->touch_3point.scale.height - coord.x
            };
            break;

          case isoLandscapeFlip:
            rotate = (ili9341_two_dimension_t){
              .x = lcd->touch_3point.scale.width - coord.x,
              .y = lcd->touch_3point.scale.height - coord.y
            };
            break;
        }

        break;

      default:
        break;
    }
  }

  return coord;
}

/************************************************************************************************************/
// GFX library 

