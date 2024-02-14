package com.rafkind.paintown;

public abstract class Lambda2{
	public abstract Object invoke( Object x, Object y ) throws Exception;

	public Object invoke_( Object x, Object y ){
		try{
			return invoke( x, y );
		} catch ( Exception e ){
		}
		return null;
	}
}
