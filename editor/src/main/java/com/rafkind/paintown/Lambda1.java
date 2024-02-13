package com.rafkind.paintown;

import java.util.List;
import java.util.Iterator;

public abstract class Lambda1{
	public abstract Object invoke( Object x ) throws Exception;

	public static void foreach( List list, Lambda1 lambda ) throws Exception {
		for ( Iterator iterator = list.iterator(); iterator.hasNext(); ){
			lambda.invoke( iterator.next() );
		}
	}
	
	public static void foreach_( List list, Lambda1 lambda ){
		try{
			foreach( list, lambda );
		} catch ( Exception e ){
		}
	}

	public Object invoke_( Object x ){
		try{
			return invoke( x );
		} catch ( Exception e ){
		}
		return null;
	}
}
