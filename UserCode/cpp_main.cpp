#include "cpp_main.hpp"


void wrap_cpp_main_init(void){
    UserCode::cpp_main_init();
}

void wrap_cpp_main_while(void){
    UserCode::cpp_main_while();
}


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){
    if(hcan == UserCode::master_can.get_handle()){
        UserCode::master_can.rx_interruption();
    }
}
void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan){
    if(hcan == UserCode::master_can.get_handle()){
        UserCode::master_can.tx_trigger();
    }
}
void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan){
    if(hcan == UserCode::master_can.get_handle()){
        UserCode::master_can.tx_trigger();
    }
}
void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan){
    if(hcan == UserCode::master_can.get_handle()){
        UserCode::master_can.tx_trigger();
    }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim == UserCode::hard_timer_1kHz.get_handle()){
        UserCode::hard_timer_1kHz.handle_callback();
    }
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c){
    if(hi2c == UserCode::as5600_i2c.get_handle()){
        UserCode::as5600_i2c.angle_update();
    }
}



namespace UserCode
{
    void cpp_main_init(void){

        master_can.filter_set_free(0);
        master_can.start();

        as5600_gpio_init();

        board_id = id_read();

        hard_timer_1kHz.set_callback(timer_interruption_1kHz);
        hard_timer_1kHz.start();

        led.set_state(1);

        pc.transmit_debug("init finished!\r\n");
    }

    void cpp_main_while(void){
        // printf debag zone
    }


    void timer_interruption_1kHz(){

        /*
        1kHzで読み取って、角度を逆正規化する
        1周期ごとの速度も出す

        生の角度情報、逆正規化した角度情報、速度をCANに乗せて送信

        正規化のリセットトリガーを設定
        */
       
        
        stmlib_v1::CanPacket angle_data;

        angle_data.is_ext_id = false;
        angle_data.is_remote_frame = false;
        angle_data.dlc = 8;

        angle_data.main_data[0] = as5600_i2c.get_raw_angle() >> 8;
        angle_data.main_data[1] = as5600_i2c.get_raw_angle() & 0x00ff;
        angle_data.main_data[2] = static_cast<uint16_t>(as5600_i2c.get_displacement_1kHz()) >> 8;
        angle_data.main_data[3] = static_cast<uint16_t>(as5600_i2c.get_displacement_1kHz()) & 0x00ff;
        angle_data.main_data[4] = as5600_i2c.get_rescaled_angle() >> 24;
        angle_data.main_data[5] = (as5600_i2c.get_rescaled_angle() >> 16) & 0x00ff;
        angle_data.main_data[6] = (as5600_i2c.get_rescaled_angle() >> 8) & 0x0000ff;
        angle_data.main_data[7] = as5600_i2c.get_rescaled_angle() & 0x000000ff;

        angle_data.packet_id = 0x100 | board_id;

        master_can.add_tx_fifo(angle_data);

        as5600_i2c.angle_update_trigger();


        while(0 < master_can.get_rx_busy_level()){
            stmlib_v1::CanPacket rx_packet = {};
            master_can.rx(rx_packet);
            if(rx_packet.packet_id == 0x200){
                as5600_i2c.reset_angle();
            }
        }
    }

    void as5600_gpio_init(){
        HAL_GPIO_WritePin(AS_DIR_GPIO_Port, AS_DIR_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(AS_PGO_GPIO_Port, AS_PGO_Pin, GPIO_PIN_RESET);
    }

    size_t id_read(){
        return HAL_GPIO_ReadPin(ID1_GPIO_Port, ID1_Pin) << 1 | HAL_GPIO_ReadPin(ID0_GPIO_Port, ID0_Pin);
    }
    
}