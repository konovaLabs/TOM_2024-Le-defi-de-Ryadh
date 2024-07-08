#ifndef __BLE_H
#define __BLE_H

typedef struct
{
    uint32_t cum_wheel_rev;
    uint16_t last_wheel_event;
} csc_measurement_t;

typedef void (*ConnectionCallback)(bool connected);

void ble_init(void);
void ble_update_send_csc_measurement(csc_measurement_t *csc_measurement);
void ble_register_connection_callback(ConnectionCallback callback);

#endif /* __BLE_H */