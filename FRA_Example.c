int FRA_Scan_Pro_Example(void)
{
	unsigned short StartFreq, EndFreq, Amp, Point;								// FRA Parameter
	unsigned char status = 0, err_code = 0;				 						// FRA Board Status, Error Code
	
	float Freq, Mag, Phase;														// FRA Result
		
	int Retry = 0;
		
	
	/* Set FRA Param ------------ */
	StartFreq = 30;
	EndFreq = 10000;
	Amp = 160;
	Point = 200;
	
	if(DW982X_FRA_Read_byte(0x00) != 0xAE )				return -1;				// Board Connection Error 
	
	PRINTF(("Start FRA Scan Pro ----------------------- \r\n"));
	
	DW982X_FRA_Write_byte(0x31, 0x08);     		// Select DW9828
	Delay_1ms(1);
	
	DW982X_FRA_Write_byte(0x30, 0x1C);     		// Salve ID 
	Delay_1ms(1);
	
	DW982X_FRA_Write_byte(0x2F, 0x0A);     		// System CLK 
	Delay_1ms(1);
		
	DW982X_FRA_Write_byte(0x03, 0x08);     		// Scan Pro Mode
	//Delay_1ms(10);
	Delay_1ms(5);
	
	DW982X_FRA_Write_2byte(0x10, 0x8000);    	// Target Position
	Delay_1ms(1);
	
	DW982X_FRA_Write_2byte(0x04, Amp);   		// Amp. Setting
	Delay_1ms(1);
	
	DW982X_FRA_Write_2byte(0x06, 0x0000);    	// Offset Setting
	Delay_1ms(1);
	
	DW982X_FRA_Write_2byte(0x0A, Point);   		// FRA Points
	Delay_1ms(1);
	
	DW982X_FRA_Write_2byte(0x0C, StartFreq);   // Start Frequency
	Delay_1ms(1);
	
	DW982X_FRA_Write_2byte(0x0C, EndFreq);     // End Frequency
	Delay_1ms(1);
	
	DW982X_FRA_Write_byte(0x01, 0x01);     // FRA Mode On
		
	/* Read Status (Timeout 5sec, interval 100ms, Check Count 50) ---------------- */
	while(status != FRA_STAT_MODE_ON)
	{
		status = DW982X_FRA_Read_byte(0x02);    // Status Check
		PRINTF(("Status %.2X \r\n", status));

		if(status == FRA_STAT_MODE_ON)
		{
			PRINTF(("FRA Ready OK \r\n"));
			Retry = 0;
			break;
		}
		else if(status== FRA_STAT_ERR)
		{
			err_code = DW982X_FRA_Read_byte(0x09);    // Status Check
			//hiu_write("FRA Ready Error - ErrCode (%.2X) \n", status_0x09);
			PRINTF(("FRA Ready Error - ErrCode (%.2X) \r\n", err_code));
			
			// FRA Stop
			DW982X_FRA_Write_byte(0x01, 0x00);
			Delay_1ms(100);
			
			return -1;
		}
		
		Delay_1ms(100);
		Retry++;
		
		if(Retry > 50)
		{
			PRINTF(("FRA Ready Timeout Error ! \r\n"));
			return -1;
		}
	}

	// FRA Start
	DW982X_FRA_Write_byte(0x01, 0x03);     // Test Start
	
	status=0;
	Retry=0;
	
	/* Read Status (Timeout 60 sec, interval 30ms, Check Count 2000) ---------------- */
	while(status != FRA_STAT_MEASURE_OK)
	{
		//Read Status
		status = DW982X_FRA_Read_byte(0x02);    // Status Check
		PRINTF(("Status %.2X \r\n", status));
		
		if(status == FRA_STAT_MEASURE_OK)
		{
			/* Read Result -------------------- */
			for(unsigned short i = 0; i < Point; i++)
			{
				DW982X_FRA_Write_2byte(0x40, i);
				
				Freq = (float)((unsigned short) DW982X_FRA_Read_2byte(0x20)/ 10); // Read Freq Value
				Mag = (float)((signed short)DW982X_FRA_Read_2byte(0x22) /10);      // Read Gain Value
				Phase = (float)((signed short)DW982X_FRA_Read_2byte(0x24) /10);        // Read Phase Value
				
				PRINTF(("idx %d %.1f Hz, %.1f dB, %.1f deg \r\n", i, Freq, Mag, Phase));
			}
			
			PRINTF(("%d points measure success \r\n", Point));
			break;
		
			Retry = 0;			
		}
		else if(status == FRA_STAT_ERR)
		{
			err_code = DW982X_FRA_Read_byte(0x09);    // Status Check
			//hiu_write("FRA Ready Error - ErrCode (%.2X) \n", status_0x09);
			PRINTF(("FRA Ready Error - ErrCode (%.2X) \r\n", err_code));
			
			// FRA Stop
			DW982X_FRA_Write_byte(0x01, 0x00);
			Delay_1ms(100);
			
			return -1;
		}
		
		Delay_1ms(30);
		Retry++;
		
		if(Retry > 2000) 
		{
			PRINTF(("FRA Measure Timeout Error ! \r\n"));
			return -1;			
		}
	}
	
	// FRA Stop
	DW982X_FRA_Write_byte(0x01, 0x00);
	Delay_1ms(100);
	
	FLAG_OFF;
	
	return 0;
}
