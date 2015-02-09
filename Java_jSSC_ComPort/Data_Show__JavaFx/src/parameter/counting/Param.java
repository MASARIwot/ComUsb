package parameter.counting;


public class Param {

	public static String tempCount(String str){
		double tmpN = Double.parseDouble(str);
		double rt = (3960)/(3.3-(tmpN*0.0032));
		tmpN = (rt - 1200)/(1200*0.00784);
		return  String.valueOf(tmpN);
		
	}
	public static String tempCO2(String str){
		double tmpN = Double.parseDouble(str);
		tmpN = (tmpN*5.22)/10; //mPPM
		return  String.valueOf(tmpN);
		
	}
	
	
}
