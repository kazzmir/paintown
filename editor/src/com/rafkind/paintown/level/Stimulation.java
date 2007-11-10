package com.rafkind.paintown.level;

import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.Token;

public abstract class Stimulation{
	public Stimulation(){
	}

	public static Stimulation load( Token t ) throws LoadException {
		Token health = t.findToken( "health" );
		if ( health != null ){
			return new HealthStimulation( health );
		}

		return null;
	}

	public abstract Stimulation copy();
	public abstract Token toToken();

	public static class HealthStimulation extends Stimulation {
		private int health;

		public HealthStimulation(){
		}

		public HealthStimulation( Token t ) throws LoadException {
			health = t.readInt( 0 );
		}

		public HealthStimulation( Stimulation copy ){
			this( (HealthStimulation) copy );
		}

		public HealthStimulation( HealthStimulation copy ){
			health = copy.health;
		}

		public int getHealth(){
			return health;
		}

		public void setHealth( int s ){
			health = s;
		}

		public Stimulation copy(){
			return new HealthStimulation( this );
		}

		public Token toToken(){
			Token t = new Token();

			t.addToken( new Token( "stimulation" ) );
			t.addToken( new String[]{ "health", String.valueOf( health ) } );

			return t;
		}
	}
}
