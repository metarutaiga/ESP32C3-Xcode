#include "esp32.h"
#include <sys/lock.h>
#include <esp_private/esp_clk.h>
#include <esp_rom_gpio.h>
#include <esp_vfs.h>
#include <esp32c3/rom/ets_sys.h>
#include <hal/gpio_ll.h>
#include <hal/uart_ll.h>
#include <lwip/sockets.h>

void esp_vfs_console_register(void)
{
}

void esp_vfs_lwip_sockets_register(void)
{
}

esp_err_t esp_vfs_register_fd_range(const esp_vfs_t* vfs, void* ctx, int min_fd, int max_fd)
{
    return ESP_FAIL;
}

int esp_vfs_select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* errorfds, struct timeval* timeout)
{
    return lwip_select(nfds, readfds, writefds, errorfds, timeout);
}

int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* errorfds, struct timeval* timeout)
{
    return lwip_select(nfds, readfds, writefds, errorfds, timeout);
}

int _fcntl_r(struct _reent* r, int fd, int cmd, int arg)
{
    return lwip_fcntl(fd, cmd, arg);
}

int _open_r(struct _reent* r, const char* path, int flags, int mode)
{
    if (strncmp(path, "/dev/console/", 13) == 0)
        return 0;
    return -1;
}

int _close_r(struct _reent* r, int fd)
{
    return lwip_close(fd);
}

off_t _lseek_r(struct _reent* r, int fd, off_t size, int mode)
{
    return 0;
}

ssize_t _read_r(struct _reent* r, int fd, void* dst, size_t size)
{
    return -1;
}

ssize_t _write_r(struct _reent* r, int fd, const void* data, size_t size)
{
    if (fd == 0) {
        static _lock_t write_lock;
        _lock_acquire_recursive(&write_lock);
        uint32_t baudrate = uart_ll_get_baudrate(&UART0, esp_clk_apb_freq());
        if (uart0_tx != U0TXD_GPIO_NUM) {
            while (uart_ll_get_txfifo_len(&UART0) < UART_LL_FIFO_DEF_LEN);
            ets_delay_us(1000);
            esp_rom_gpio_connect_out_signal(U0TXD_GPIO_NUM, UART_PERIPH_SIGNAL(0, SOC_UART_TX_PIN_IDX), 0, 0);
            uart_ll_set_baudrate(&UART0, 115200, esp_clk_apb_freq());
            uart_ll_txfifo_rst(&UART0);
        }
        const char* text = data;
        for (size_t i = 0; i < size; ++i) {
            uint8_t c = text[i];
            if (c == '\n') {
                c = '\r';
                while (uart_ll_get_txfifo_len(&UART0) < 2);
                uart_ll_write_txfifo(&UART0, &c, 1);
                c = '\n';
            }
            while (uart_ll_get_txfifo_len(&UART0) < 2);
            uart_ll_write_txfifo(&UART0, &c, 1);
        }
        if (uart0_tx != U0TXD_GPIO_NUM) {
            while (uart_ll_get_txfifo_len(&UART0) < UART_LL_FIFO_DEF_LEN);
            ets_delay_us(1000);
            esp_rom_gpio_connect_out_signal(uart0_tx, UART_PERIPH_SIGNAL(0, SOC_UART_TX_PIN_IDX), 0, 0);
            uart_ll_set_baudrate(&UART0, baudrate, esp_clk_apb_freq());
            uart_ll_txfifo_rst(&UART0);
        }
        _lock_release_recursive(&write_lock);
        return size;
    }
    return -1;
}

int _fstat_r(struct _reent* r, int fd, struct stat* st)
{
    if (fd == 0) {
        st->st_mode = S_IFCHR;
        return 0;
    }
    if (fd >= LWIP_SOCKET_OFFSET) {
        st->st_mode = S_IFSOCK;
        return 0;
    }
    return -1;
}

int _stat_r(struct _reent* r, const char* path, struct stat* st)
{
    return -1;
}

int _link_r(struct _reent* r, const char* n1, const char* n2)
{
    return -1;
}

int _unlink_r(struct _reent* r, const char* path)
{
    return -1;
}

int _rename_r(struct _reent* r, const char* src, const char* dst)
{
    return -1;
}
