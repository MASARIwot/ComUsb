package com;

//import java.util.ArrayList;
import java.util.LinkedList;
import java.util.NoSuchElementException;
import java.util.Queue;

import enums.Parametr;



public class Session{
	
	private volatile static  Session instance ;//= new SingletonDoubleCheckedLocking();  //instance-приклад//= new Singleton();
	/*
	 * volatile:
	 * The value of this variable will never be cached thread-locally: all reads and writes will go straight to "main memory";
		Access to the variable acts as though it is enclosed in a synchronized block, synchronized on itself.
	 */
	private Session(){
		
	}
	public static Session getInstance(){
		if(instance == null){
			synchronized (Session.class) {
				if(instance == null) instance = new Session();
				
			}
		}
		return instance; // or Singleton.instance
	}
	/*Main Program*/
	private StringBuilder buff = new StringBuilder();
//	private ArrayList<String> ressArr = new ArrayList<String>();
	private Queue<String> queueTemperture =  new LinkedList<String>();
	private Queue<String> queueHamidity =  new LinkedList<String>();
	private Queue<String> queueC02 =  new LinkedList<String>();
	
	private Parametr option = Parametr.NON;
	
	public synchronized void setData(String str)	{
		
		if(str.contains("s"))      { buff = new StringBuilder();  option = Parametr.TEMPERATURE; }
		else if(str.contains("w")) { buff = new StringBuilder();  option = Parametr.HAMIDY; }
		else if(str.contains("c")) { buff = new StringBuilder();  option = Parametr.CO2; }
		else if(str.contains("e")){
			addParameters();
			option = Parametr.NON;
		}
		else buff.append(str);
			
	}
	public  String getDataTemperture() throws NoSuchElementException{
		 if (queueTemperture.size() == 0) throw new NoSuchElementException();
		return queueTemperture.remove();
	}
	public  String getDataHamidity() throws NoSuchElementException{
		 if (queueHamidity.size() == 0) throw new NoSuchElementException();
		return queueHamidity.remove();
	}
	public  String getDataC02() throws NoSuchElementException{
		 if (queueC02.size() == 0) throw new NoSuchElementException();
		return queueC02.remove();
	}
	private void addParameters(){
		
		    if(option == Parametr.TEMPERATURE){
				buff.append(" "); 
				String str2 = buff.toString(); 
				queueTemperture.offer(str2); 
				System.out.println(str2 + "T"); 
				} 
			if(option == Parametr.CO2){
				buff.append(" "); 
				String str2 = buff.toString(); 
				queueC02.offer(str2); 
				System.out.println(str2 + "C"); 
				} 
			if(option == Parametr.HAMIDY){
				buff.append(" "); 
				String str2 = buff.toString(); 
				queueHamidity.offer(str2); 
				System.out.println(str2 + "H"); 
				}
			if(option == Parametr.NON){
				 
				}
		
	}
	

}
