package com;

import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;

public class PortReader implements SerialPortEventListener {
	
	private SaveData saveData = SaveData.getInstance();
	SerialPort serialPort;
	
	public PortReader(SerialPort serialPort){
		this.serialPort = serialPort;
	}
	@Override
	public void serialEvent(SerialPortEvent event) {
		if(event.isRXCHAR() && event.getEventValue() > 0){
            try {
            	//Получаем ответ от устройства, обрабатываем данные и т.д.
            	//byte[] buffer = serialPort.readBytes(10); 
            	String data = serialPort.readString(event.getEventValue());	
            	   		
//                    System.out.print(data);
//                    System.out.println();
                    saveData.setData(data);
            	   

            }
            catch (SerialPortException  ex) {
                System.out.println(ex);
            }
        }
		
	}/*serialEvent*/
	
	
	public void stopCom() throws SerialPortException
	{
		try {
			serialPort.closePort();
		} catch (SerialPortException e) {
			// TODO Auto-generated catch block
			throw new SerialPortException(null, null, null);
		}
	}
	
	
}/*PortReader*/