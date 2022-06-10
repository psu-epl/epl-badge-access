#include "fake_reader.h"
void fakeBadgeISR(void *arg);

FakeReader::FakeReader(gpio_num_t buttonGPIO, esp_event_loop_handle_t event_loop) : ButtonGPIO_(buttonGPIO),
                                                                                    event_loop_(event_loop)
{
    esp_err_t err = gpio_install_isr_service(0);
    if (err)
    {
        log_e("gpio_install_isr_service() error: %s", esp_err_to_name(err));
    }

    err = gpio_isr_handler_add(ButtonGPIO_, fakeBadgeISR, (void *)this);
    if (err)
    {
        log_e("gpio_isr_handler_add() error: %s", esp_err_to_name(err));
    }
}

esp_event_loop_handle_t FakeReader::getEventLoop()
{
    return event_loop_;
}

void fakeBadgeISR(void *arg)
{
    String fakeBadge = "8675309";

    FakeReader *that = (FakeReader *)arg;
    esp_event_post_to(that->getEventLoop(), LabpassFakeReaderEvent, labpassFakeReaderEventType::shortBadge, (void *) &fakeBadge, sizeof(&fakeBadge), portMAX_DELAY);
}
