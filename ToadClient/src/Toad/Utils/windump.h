#pragma once

namespace Utils 
{
	// Handle unhandled exceptions by writing a mini dump
	// Use with	SetUnhandledExceptionFilter(UnhandledExceptionHandler);
	LONG WINAPI UnhandledExceptionHandler(struct _EXCEPTION_POINTERS* ep);
}

