package com.bignerdranch.android.geoquiz;

/**
 * Created by samsung on 2015/2/28.
 */
public class TrueFalse {
    private int mQuestion;
    private boolean mTrueQuestion;
    public TrueFalse(int question, boolean trueQuestion) {
        mQuestion = question;
        mTrueQuestion = trueQuestion;
    }

    public int getmQuestion() {
        return mQuestion;
    }

    public void setmQuestion(int mQuestion) {
        this.mQuestion = mQuestion;
    }

    public boolean ismTrueQuestion() {
        return mTrueQuestion;
    }

    public void setmTrueQuestion(boolean mTrueQuestion) {
        this.mTrueQuestion = mTrueQuestion;
    }
}
