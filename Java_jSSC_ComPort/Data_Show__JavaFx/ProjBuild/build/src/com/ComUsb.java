package com;



import jssc.SerialPort;
import jssc.SerialPortException;
import jssc.SerialPortList;

public  class ComUsb {
	private SerialPort serialPort;
	private PortReader portReader;
	@SuppressWarnings("unused")
	private SaveData saveData = SaveData.getInstance();
    
	public ComUsb() {
		
	} 
	
	public String[] getSerialPortList() {
		String[] portName = SerialPortList.getPortNames();
		
		return portName;
	}
	
	public void configurationPortAndStart(/*String port ,int BAUDRATE_9600, int DATABITS_8, int STOPBITS_1, int PARITY_NONE */)
	{
		//Передаём в конструктор имя порта
        serialPort = new SerialPort("COM5");
        
        System.out.println("COM5");
        try {
            //Открываем порт
            serialPort.openPort();
            //Выставляем параметры
           // serialPort.setParams(BAUDRATE_9600,DATABITS_8,STOPBITS_1,STOPBITS_1);//Set params
            serialPort.setParams(SerialPort.BAUDRATE_9600,
                                 SerialPort.DATABITS_8,
                                 SerialPort.STOPBITS_1,
                                 SerialPort.PARITY_NONE);
            //Включаем аппаратное управление потоком
            portReader = new PortReader(serialPort);
            serialPort.setFlowControlMode(SerialPort.FLOWCONTROL_RTSCTS_IN | 
                                          SerialPort.FLOWCONTROL_RTSCTS_OUT);
            //Устанавливаем ивент лисенер и маску
            serialPort.addEventListener(portReader, SerialPort.MASK_RXCHAR);
            //Отправляем запрос устройству
            //serialPort.writeString("Get data");
           // serialPort.closePort();
        }
        catch (SerialPortException ex) {
            System.out.println(ex);
        }
				
	}
	
	public void stopCom() throws Exception
	{
		try {
			//serialPort.closePort();
			portReader.stopCom();
		} catch (SerialPortException e) {
			// TODO Auto-generated catch block
			throw new Exception("Problem with port" + e.getMessage());
		}
	}



	

}
