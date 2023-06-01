#ifndef R_SCI_UART_CFG_H_
#define R_SCI_UART_CFG_H_
#ifdef __cplusplus
extern "C" {
#endif

#define SCI_UART_CFG_PARAM_CHECKING_ENABLE (BSP_CFG_PARAM_CHECKING_ENABLE)
#define SCI_UART_CFG_FIFO_SUPPORT (0)
#define SCI_UART_CFG_DTC_SUPPORTED (0)
#define SCI_UART_CFG_FLOW_CONTROL_SUPPORT (0)
#define SCI_UART_CFG_RS485_SUPPORT (0)

// Followings are added to use R_SCI_UART_BaudCalculate function only.
// TX/RX in r_sci_uart.c are disabled to avoid conflict with ra/ra_sci.c's interrupt handlers,
// and unused-variable & unused-function are ignored when compiling.
#define SCI_UART_CFG_TX_ENABLE (0)
#define SCI_UART_CFG_RX_ENABLE (0)
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#ifdef __cplusplus
}
#endif
#endif /* R_SCI_UART_CFG_H_ */
