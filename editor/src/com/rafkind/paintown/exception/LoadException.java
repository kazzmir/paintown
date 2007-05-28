package com.rafkind.paintown.exception;

public class LoadException extends EditorException {

	public LoadException(){
	}

	public LoadException( String s ){
		super( s );
	}

	public LoadException( String s, Exception e ){
		super( s, e );
	}
}
