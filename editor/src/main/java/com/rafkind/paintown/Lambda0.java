package com.rafkind.paintown;

public abstract class Lambda0{
	public abstract Object invoke() throws Exception;

	public Object invoke_(){
		try{
			return invoke();
		} catch ( Exception e ){
		}
		return null;
	}
}
