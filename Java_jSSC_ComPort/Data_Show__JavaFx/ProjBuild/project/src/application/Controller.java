package application;

import java.net.URL;
import java.util.NoSuchElementException;
import java.util.ResourceBundle;

import com.ComUsb;
import com.SaveData;

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
//	private boolean statusOfApdate = false;

	

	private int counter = 0;
	private SaveData saveData = SaveData.getInstance();
	private ComUsb comData = new ComUsb(); 
	private ObservableList<XYChart.Series<Integer, Double>> lineChartData = FXCollections.observableArrayList();
	private LineChart.Series<Integer, Double> series1 = new LineChart.Series<Integer, Double>();
		 
	@Override
	public void initialize(URL location, ResourceBundle resources) {
			series1.setName("Data");
		    lineChartData.add(series1);
	        dataChar.setData(lineChartData);
	        dataChar.createSymbolsProperty();
	    }
	private synchronized void upDate(){
		
			try{
				
				while(isStatus()){
					
				 String str = saveData.getData();
				 if(str == null) setStatus();
				 else series1.getData().add(new XYChart.Data<Integer, Double>(counter++, Double.parseDouble(str)));
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
//	private void reversStatusOfApdate() {
//		this.statusOfApdate = !statusOfApdate;
//		
//	}
//	private boolean getStatusOfApdate(){
//		return this.statusOfApdate;
//	}
	@FXML
	public void clickButtonSave(){
		
		try {
			comData.stopCom();
		} catch (Exception e) {
			
			e.printStackTrace();
		}
	}
		
	@FXML
	public void clickButtonStart(){
		
		Thread my = new Thread(new Runnable() {
			
			@Override
			public void run() {
				comData.configurationPortAndStart();
				
			}
		});
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
//		reversStatusOfApdate();
		editionalMe();
	}
	private void editionalMe(){
		
		upDate();
					
	}


}
