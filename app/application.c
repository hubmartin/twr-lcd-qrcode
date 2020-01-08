#include <application.h>

#include "qrcodegen.h"

// LED instance
bc_led_t led;

// Button instance
bc_button_t button;

bc_gfx_t *gfx;

void qrcode_project(char *project_name);


const char *str_urls[] =
{
    "https://www.hardwario.com/",
    "https://www.hackster.io/hardwario",
    "http://hub.local",
    /*
        "Alice was beginning to get very tired of sitting by her sister on the bank, "
			"and of having nothing to do: once or twice she had peeped into the book her sister was reading, "
			"but it had no pictures or conversations in it, 'and what is the use of a book,' thought Alice "
			"'without pictures or conversations?' So she was considering in her own mind (as well as she could, "
			"for the hot day made her feel very sleepy and stupid), whether the pleasure of making a "
			"daisy-chain would be worth the trouble of getting up and picking the daisies, when suddenly "
			"a White Rabbit with pink eyes ran close by her.",*/
    NULL
};

int project_index = 0;

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    if (event == BC_BUTTON_EVENT_PRESS)
    {
        project_index++;

        if(str_urls[project_index] == NULL)
        {
            project_index = 0;
        }

        qrcode_project((char*)str_urls[project_index]);
    }
}


static void print_qr(const uint8_t qrcode[])
{
    bc_gfx_clear(gfx);

    bc_gfx_set_font(gfx, &bc_font_ubuntu_13);
    bc_gfx_draw_string(gfx, 2, 0, (char*)str_urls[project_index], true);

    uint32_t offset_x = 8;
    uint32_t offset_y = 32;
    uint32_t box_size = 2;
	int size = qrcodegen_getSize(qrcode);
	int border = 4;
	for (int y = -border; y < size + border; y++) {
		for (int x = -border; x < size + border; x++) {
			//fputs((qrcodegen_getModule(qrcode, x, y) ? "##" : "  "), stdout);
            uint32_t x1 = offset_x + x * box_size;
            uint32_t y1 = offset_y + y * box_size;
            uint32_t x2 = x1 + box_size;
            uint32_t y2 = y1 + box_size;

            bc_gfx_draw_fill_rectangle(gfx, x1, y1, x2, y2, qrcodegen_getModule(qrcode, x, y));
		}
		//fputs("\n", stdout);
	}
	//fputs("\n", stdout);
    bc_gfx_update(gfx);
}

void qrcode_project(char *text)
{
    bc_system_pll_enable();

	// Make and print the QR Code symbol
	static uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];
	static uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];
	bool ok = qrcodegen_encodeText(text, tempBuffer, qrcode, qrcodegen_Ecc_LOW,	qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX, qrcodegen_Mask_AUTO, true);

	if (ok)
    {
		print_qr(qrcode);
    }

    bc_system_pll_disable();
}

void application_init(void)
{
    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_set_mode(&led, BC_LED_MODE_ON);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    bc_module_lcd_init();
    gfx = bc_module_lcd_get_gfx();

    qrcode_project((char*)str_urls[project_index]);
}
