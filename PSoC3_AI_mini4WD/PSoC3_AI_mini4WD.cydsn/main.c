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

int main()
{
	PWM_Start();
    CyGlobalIntEnable; /* Enable global interrupts. */

	PWM_WriteCompare1(0);
	PWM_WriteCompare2(0);
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
