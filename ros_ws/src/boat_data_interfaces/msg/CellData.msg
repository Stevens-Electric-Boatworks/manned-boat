# NOTE: A lot of the info in this message is technical output from the
# Telit LE910C4-NF cell modem.
# See its manual for more information about specifics:
# https://sixfab.com/wp-content/uploads/2022/02/Telit_LE910Cx_AT_Commands_Reference_Guide_r12.pdf


# Carrier name (e.g. "AT&T")
string network

# Carrier technology (e.g. "LTE" or "3G")
# NOTE: 2G and 3G networks no longer exist in the U.S., so this will pretty
# much always be "LTE". The HAT does not support 5G.
string technology

# The number of cell signal "bars" to display.
# Range from 0 - 4, or 255 for "No Signal"
uint8 bars

# Raw RSRP value from modem (ONLY APPLIES TO 4G NETWORKS)
# RSRP - Reference Signal Received Power
uint8 rsrp

# Raw RSRQ value from modem (ONLY APPLIES TO 4G NETWORKS)
# Reference Signal Received Quality
uint8 rsrq

# Network Registration Status
uint8 reg_status

# IP Address on network
string ip_addr

# Carrier APN (Access Point Name)
string apn

# Authentication (PIN) Status
string pin_status