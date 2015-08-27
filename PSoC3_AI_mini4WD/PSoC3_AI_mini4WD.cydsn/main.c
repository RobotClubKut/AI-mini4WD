/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include<stdio.h>
#define BUFFER_SIZE 900

bit isr_flag = 0;
CY_ISR(isr_int)
{
    isr_flag = 1;
}
int main()
{
    int F=250*0.50;
    uint8 select = 1;
    int16 left, right;
    int16 sensData[2][BUFFER_SIZE];
    uint16 i = 0;
    uint16 j = 0;
    
    uint8 msg[32];
    
    UART_Start();
    isr_StartEx(isr_int);
	PWM_Start();
    AMux_Start();
    AMux_Connect(0);
    AMux_Disconnect(1);
    ADC_DelSig_Start();
    ADC_DelSig_StartConvert();
    
    CyGlobalIntEnable; /* Enable global interrupts. */
	INH_Write(0);
	PWM_WriteCompare1(0);
	PWM_WriteCompare2(0);
	
	CyDelay(1000);
	
	INH_Write(1);
	PWM_WriteCompare1(F);
	PWM_WriteCompare2(0);
    for(;;)
    {
        /* Place your application code here. */
        if(ADC_DelSig_IsEndConversion(ADC_DelSig_RETURN_STATUS)){
            if(select){
                right = ADC_DelSig_GetResult16();
                ADC_DelSig_StopConvert();
                AMux_Disconnect(0);
                AMux_Connect(1);
                ADC_DelSig_StartConvert();
                select = 0;
            }
            else{
                left = ADC_DelSig_GetResult16();
                ADC_DelSig_StopConvert();
                AMux_Disconnect(1);
                AMux_Connect(0);
                ADC_DelSig_StartConvert();
                select = 1;
            }
        }
        if(isr_flag)
        {
            sensData[0][i] = left;
            sensData[1][i] = right;
            
            //sprintf(msg,"%d,%d,%d\n",j,left,right);
            //UART_PutString(msg);
            if(i < BUFFER_SIZE)
            {
                i++;
            }
            else
            {
                PWM_WriteCompare1(0);
	            PWM_WriteCompare2(0);            
                if((left < 3400) && (right < 3400)){
                    for(j = 0;j < BUFFER_SIZE;j++){
                        sprintf(msg,"%d,%d,%d\n",j,sensData[0][j],sensData[1][j]);
                        UART_PutString(msg);
                    }
                    while(1);
                }
            }
            isr_flag = 0;
        }
    }
}

/* [] END OF FILE */
