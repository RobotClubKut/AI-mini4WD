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

uint8 AI_mini4WD_GetAxi(uint16* axi_x,uint16* axi_y,uint16* axi_z)
{
	static uint8 select = 1;
	
	if(ADC_DelSig_IsEndConversion(ADC_DelSig_RETURN_STATUS)){
        // 横のデータ
		if(select == 1){
            *axi_x = ADC_DelSig_GetResult16();
            ADC_DelSig_StopConvert();
            AMux_Disconnect(0);
            AMux_Disconnect(2);
            AMux_Connect(1);
            ADC_DelSig_StartConvert();
            select = 2;
        }
		// 縦のデータ
        else if(select == 2)
		{
            *axi_y = ADC_DelSig_GetResult16();
            ADC_DelSig_StopConvert();
            AMux_Disconnect(0);
            AMux_Disconnect(1);
			AMux_Connect(2);
            ADC_DelSig_StartConvert();
            select = 3;
        }
		//高さのデータ
		else if(select == 3)
		{
            *axi_z = ADC_DelSig_GetResult16();
            ADC_DelSig_StopConvert();
            AMux_Disconnect(1);
            AMux_Disconnect(2);
            AMux_Connect(0);
            ADC_DelSig_StartConvert();
            select = 1;
        }
		return 0;
    }
	else
	{
		return 1;
	}
}

void AI_mini4WD_Init(uint16* axi_x_fir,uint16* axi_y_fir,uint16* axi_z_fir,uint16 delay)
{
	uint8 i,j;
	
	UART_Start();
    isr_StartEx(isr_int);
	PWM_Start();
    AMux_Start();
    AMux_Connect(0);
    AMux_Disconnect(1);
    AMux_Disconnect(2);
    ADC_DelSig_Start();
    ADC_DelSig_StartConvert();
    
    CyGlobalIntEnable; /* Enable global interrupts. */
	INH_Write(0);
	PWM_WriteCompare1(0);
	PWM_WriteCompare2(0);
	
	for(i = 1;i < 17;i++)
	{
		for(j = 0;j < 3;j++){
			while(AI_mini4WD_GetAxi(&(axi_x_fir[i]),&(axi_y_fir[i]),&(axi_z_fir[i])) == 1);		
		}
	}
	CyDelay(delay);
}


int main()
{
    int F=250*0.40;
    uint16 axi_x=0,axi_y=0,axi_z=0;
    uint16 sensData[2][BUFFER_SIZE];
    uint16 i = 0;
    uint16 j = 0;
    
	uint16 axi_x_fir[17];
	uint16 axi_y_fir[17];
	uint16 axi_z_fir[17];
	uint8 axi_counter = 1;
    uint8 axi_fir_i;
	uint8 msg[32];   
	
	AI_mini4WD_Init(axi_x_fir,axi_y_fir,axi_z_fir,1000);
	
	INH_Write(1);
	PWM_WriteCompare1(F);
	PWM_WriteCompare2(0);
    for(;;)
    {
        /* Place your application code here. */
        AI_mini4WD_GetAxi(&axi_x,&axi_y,&axi_z);
        if(isr_flag)
        {
            //sprintf(msg,"%5d,%5d,%5d\n",(int)axi_x-2158,(int)axi_y-2048,(int)axi_z-2048);
            //UART_PutString(msg);
			axi_x_fir[axi_counter] = axi_x;
			axi_y_fir[axi_counter] = axi_y;
			axi_z_fir[axi_counter] = axi_z;
			axi_x_fir[0] = axi_y_fir[0] = axi_z_fir[0] = 0;
			for(axi_fir_i = 1;axi_fir_i < 17;axi_fir_i++)
			{
				axi_x_fir[0] += axi_x_fir[axi_fir_i];
				axi_y_fir[0] += axi_y_fir[axi_fir_i];
				axi_z_fir[0] += axi_z_fir[axi_fir_i];
			}
			axi_x_fir[0] >>= 4;
			axi_y_fir[0] >>= 4;
			axi_z_fir[0] >>= 4;
			
			if(axi_counter < 17)
			{
				axi_counter++;
			}
			else
			{
				axi_counter = 1;
			}
			
            if(i < BUFFER_SIZE)
            {
				sensData[0][i] = axi_x_fir[0];
            	sensData[1][i] = axi_z_fir[0];
				
                i++;
            }
            else
            {
                PWM_WriteCompare1(0);
	            PWM_WriteCompare2(0);
				if(DataFlag_Read() == 1){
                    for(j = 0;j < BUFFER_SIZE;j++){
                        sprintf(msg,"%d,%d,%d\n",(int)(j*5),(int)sensData[0][j],(int)sensData[1][j]);
                        UART_PutString(msg);
                    }
                    while(DataFlag_Read() == 1);
					
                }
            }
            isr_flag = 0;
        }
    }
}

/* [] END OF FILE */
