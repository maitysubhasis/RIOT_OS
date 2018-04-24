include ../Makefile.tests_common

BOARD = openmote-cc2538

BOARD_INSUFFICIENT_MEMORY := msb-430 msb-430h nucleo-f334 nucleo-l053 \
                             nucleo32-f031 nucleo32-f042 nucleo32-l031 \
                             stm32f0discovery telosb wsn430-v1_3b wsn430-v1_4 \
                             z1

CFLAGS += DGNRC_NETIF_NUMOF=2

USEMODULE += auto_init_gnrc_netif
USEMODULE += enc28j60
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# set board specific peripheral configurations
ifneq (,$(filter stm32f4discovery,$(BOARD)))
  ENC_SPI ?= SPI_DEV\(1\)
  ENC_CS  ?= GPIO_PIN\(PORT_B,12\)
  ENC_INT ?= GPIO_PIN\(PORT_B,11\)
  ENC_RST ?= GPIO_PIN\(PORT_B,10\)
endif

ifneq (,$(filter openmote-cc2538,$(BOARD)))
  ENC_SPI ?= SPI_DEV\(0\)
  ENC_CS  ?= GPIO_PIN\(0,3\) # chip select pin (GPIO) , done
  ENC_INT ?= GPIO_PIN\(3,0\) # https://sourceforge.net/p/contiki/mailman/message/33525505/
  ENC_RST ?= GPIO_PIN\(1,3\) # manually checked
endif

# fallback: set SPI bus and pins to default values
ENC_SPI ?= SPI_DEV\(0\)
ENC_CS  ?= GPIO_PIN\(0,0\)
ENC_INT ?= GPIO_PIN\(0,1\)
ENC_RST ?= GPIO_PIN\(0,2\)
# export SPI and pins
CFLAGS += -DENC28J60_PARAM_SPI=$(ENC_SPI)
CFLAGS += -DENC28J60_PARAM_CS=$(ENC_CS)
CFLAGS += -DENC28J60_PARAM_INT=$(ENC_INT)
CFLAGS += -DENC28J60_PARAM_RESET=$(ENC_RST)

# make sure we read the local enc28j60 params file
CFLAGS += -I$(CURDIR)

include $(RIOTBASE)/Makefile.include
