package com.rightware.kanzi.engine;

/**
 * Copyright 2008-2011 by Rightware. All rights reserved.
 */
public class EngineException extends Exception {
    private final int errorCode;

    public EngineException(int errorCode) {
        super("Error code is " + errorCode);
        this.errorCode = errorCode;
    }

    public int getErrorCode() {
        return errorCode;
    }
}
