package com.rafkind.paintown.exception;

public class EditorException extends Exception {

	public EditorException(){
	}

	public EditorException( String s ){
		super( s );
	}

	public EditorException( String s, Exception e ){
		super( s, e );
	}
}
