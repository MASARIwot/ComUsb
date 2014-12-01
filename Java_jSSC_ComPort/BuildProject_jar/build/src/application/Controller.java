package application;

import java.net.URL;
import java.util.NoSuchElementException;
import java.util.ResourceBundle;

import parameter.counting.Param;

import com.ComUsb;
import com.Session;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.chart.LineChart;
import javafx.scene.chart.XYChart;
import javafx.scene.control.Button;
import javafx.scene.control.TextField;

public class Controller implements Initializable {
	
	@FXML private Button buttonStart;
	@FXML private Button buttonSave;
	@FXML private TextField textFildcomList;
	@FXML private LineChart<Integer, Double> dataChar;
	@FXML private Button buttonUpdate;
	private boolean status = true;
	private int counterTemperature = 0;
	private Session sessions = Session.getInstance();
	private ComUsb comData = new ComUsb(); 
	private ObservableList<XYChart.Series<Integer, Double>> lineChartData = FXCollections.observableArrayList();
	private LineChart.Series<Integer, Double> seriesTemperature = new LineChart.Series<Integer, Double>();
	private LineChart.Series<Integer, Double> seriesHamidity = new LineChart.Series<Integer, Double>();
	private LineChart.Series<Integer, Double> seriesCO2 = new LineChart.Series<Integer, Double>();
		 
	@SuppressWarnings("unchecked")
	@Override
	public void initialize(URL location, ResourceBundle resources) {
		    seriesTemperature.setName("Temperature");
		    seriesHamidity.setName("Hamidity");
		    seriesCO2.setName("CO2");
		   
		   lineChartData.addAll(seriesTemperature ,seriesHamidity,seriesCO2);
           dataChar.setData(lineChartData);
	       dataChar.isResizable();
	     
	    }
	private synchronized void upDate(){
		try{
			
			while(isStatus()){
				 counterTemperature++;
				 String strTemperatur = Param.tempCount(sessions.getDataTemperture());// sessions.getDataTemperture();
				 String strHamidity =  sessions.getDataHamidity();
				 String strCO2 = Param.tempCO2(sessions.getDataC02());
				 if(strTemperatur == null && strHamidity == null && strCO2 == null) {setStatus();}
				 if(strTemperatur != null) seriesTemperature.getData().add(new XYChart.Data<Integer, Double>(counterTemperature, Double.parseDouble(strTemperatur)));
				 if(strHamidity != null)  seriesHamidity.getData().add(new XYChart.Data<Integer, Double>(counterTemperature, Double.parseDouble(strHamidity)));
				 if(strCO2 != null) seriesCO2.getData().add(new XYChart.Data<Integer, Double>(counterTemperature, Double.parseDouble(strCO2)));
			}
			setStatus();
			}catch (NoSuchElementException | NumberFormatException e) {
				
			}
			
      }		

	/**
	 * @return the status
	 */
	private boolean isStatus() {
		return status;
	}
	/**
	 * @param status the status to set
	 */
	private void setStatus() {
		this.status = !status;
	}
	/**
	 * @param statusOfApdate the statusOfApdate to set
	 */

	
	@FXML
	public void clickButtonSave(){
		seriesTemperature.getData().clear();
		seriesHamidity.getData().clear();
		seriesCO2.getData().clear();
		try {
			comData.stopCom();
		} catch (Exception e) {
			
			e.printStackTrace();
		}
	}
		
	@FXML
	public void clickButtonStart(){
		
		Thread my = new Thread(new Runnable() { @Override public void run() {comData.configurationPortAndStart();}});
		my.setDaemon(true);
		my.start();
		String[] comList = comData.getSerialPortList();
		String com = " ";
		if(comList.length !=0){
			for(int i=0 ; i < comList.length; i++){
				com += comList[i];
			}
			textFildcomList.setText(com);
		}
		upDate();
	}
	@FXML
	public void clickButtonUpdate()
	{
		editionalMe();
	}
	private void editionalMe(){
		upDate(); //:)
					
	}


}

