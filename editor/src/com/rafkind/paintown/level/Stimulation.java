package com.rafkind.paintown.level;

import com.rafkind.paintown.exception.LoadException;
import com.rafkind.paintown.Token;

public abstract class Stimulation{
	public Stimulation(){
	}

	public static Stimulation load(Token t) throws LoadException {
		Token health = t.findToken("health");
		if (health != null){
			return new HealthStimulation(health);
		}
        Token speed = t.findToken("speed");
        if (speed != null){
            return new SpeedStimulation(speed);
        }
        Token invincibility = t.findToken("invincibility");
        if (invincibility != null){
            return new InvincibilityStimulation(invincibility);
        }

		return null;
	}

	public abstract Stimulation copy();
	public abstract Token toToken();

	public static class HealthStimulation extends Stimulation {
		private int health;

		public HealthStimulation(){
		}

		public HealthStimulation(Token t) throws LoadException {
			health = t.readInt(0);
		}

		public HealthStimulation(HealthStimulation copy){
			health = copy.health;
		}

		public int getHealth(){
			return health;
		}

		public void setHealth( int s ){
			health = s;
		}

		public Stimulation copy(){
			return new HealthStimulation(this);
		}

		public Token toToken(){
			Token t = new Token();

			t.addToken(new Token("stimulation"));
			t.addToken(new String[]{"health", String.valueOf(health)});

			return t;
		}
	}

    public static class InvincibilityStimulation extends Stimulation {
        private int duration;

		public InvincibilityStimulation(){
		}

		public InvincibilityStimulation(Token t) throws LoadException {
			duration = t.readInt(0);
		}

		public InvincibilityStimulation(InvincibilityStimulation copy){
			duration = copy.duration;
		}

		public int getDuration(){
			return duration;
		}

		public void setDuration(int duration){
            this.duration = duration;
		}

		public Stimulation copy(){
			return new InvincibilityStimulation(this);
		}

		public Token toToken(){
			Token t = new Token();

			t.addToken(new Token("stimulation"));
			t.addToken(new String[]{"invincibility", String.valueOf(duration)});

			return t;
		}
    }

    public static class SpeedStimulation extends Stimulation {
        private double boost;
        private int ticks;

		public SpeedStimulation(){
		}

		public SpeedStimulation(Token t) throws LoadException {
			boost = t.readDouble(0);
			ticks = t.readInt(1);
		}

		public SpeedStimulation(SpeedStimulation copy){
            boost = copy.boost;
            ticks = copy.ticks;
		}

        public double getBoost(){
			return boost;
		}

        public int getTicks(){
            return ticks;
        }

        public void setBoost(double boost){
            this.boost = boost;
        }

        public void setTicks(int ticks){
            this.ticks = ticks;
        }

		public Stimulation copy(){
			return new SpeedStimulation(this);
		}

		public Token toToken(){
			Token t = new Token();

			t.addToken(new Token("stimulation"));
			t.addToken(new String[]{"speed", String.valueOf(boost), String.valueOf(ticks)});

			return t;
		}
    }
}
