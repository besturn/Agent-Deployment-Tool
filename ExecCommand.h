//====================================================================================
// Open Computer and Software Inventory Next Generation
// Copyright (C) 2010 OCS Inventory NG Team. All rights reserved.
// Web: http://www.ocsinventory-ng.org

// This code is open source and may be copied and modified as long as the source
// code is always made freely available.
// Please refer to the General Public Licence V2 http://www.gnu.org/ or Licence.txt
//====================================================================================

// ExecCommand.h: interface for the CExecCommand class.
//
// This class is inspired from CMake (Cross-Platform Makefile Generator)
// cmWin32ProcessExecution class, but is updated to be Unicode compliant
//////////////////////////////////////////////////////////////////////

#pragma once

// Command execution function result
#define EXEC_SUCCESSFULL			0	// Excute success
#define EXEC_ERROR_START_COMMAND	1	// Failed to launch command
#define EXEC_ERROR_WAIT_COMMAND		2	// Failed to wait command termination or to get exit code
// Time to wait in ms between each check for alive process
#define EXEC_WAIT_CHECK_LATENCY		1000	

class CExecCommand
{
public:
	CExecCommand();
	~CExecCommand();

  /**
   Set process execution timeout, in milliseconds
   By default, wait is INFINITE
   */
  void setTimeout( DWORD dwTimeout = INFINITE);

  /**
   Use COMSPEC environnement variable to start process (e.g cmd.exe /C command) 
   or not (Does not allow searching in path for command => must provide full path
   for command)
   */
   void useComspec( BOOL bUse = TRUE);

  /**
   * Exec command in the directory path. Make sure that the
   * executable is either in the path or specify the full path.
   * We don't care about command ending, so stdout and stderr
   * will not be grabbed, and we will not try to get process
   * exit code
   * Return EXEC_SUCCESSFULL if all successfull
   *        EXEC_ERROR_START_COMMAND if start command error
   */
  int execNoWait(LPCTSTR lpstrCommand, LPCTSTR lpstrPath);

  /**
   * Exec command in the directory path. Make sure that the
   * executable is either in the path or specify the full path.
   * We will try to get process exit code (use getExitCode() to retreive it)
   * stdout and stderr will be grabbed (use getOutput() to retreive it)
   * For this, we will wait until command end (but if command starts other
   * processes/threads, we will not be able to wait for those ending)
   * Return EXEC_SUCCESSFULL if all successfull
   *        EXEC_ERROR_START_COMMAND if start command error
   *        EXEC_ERROR_WAIT_COMMAND if wait or get exit code/ouput error,
   */
  int execWait( LPCTSTR lpstrCommand, LPCTSTR lpstrPath);

  /**
   * Exec command in the directory path. Make sure that the
   * executable is either in the path or specify the full path.
   * Wait for command AND ALL others threads/processes it may launch to finish.
   * In this case, we try get exit code, but NOT output
   * Return EXEC_SUCCESSFULL if all successfull
   *        EXEC_ERROR_START_COMMAND if start command error
   *        EXEC_ERROR_WAIT_COMMAND if wait error,
   */
  int execWaitForAllChilds( LPCTSTR lpstrCommand, LPCTSTR lpstrPath);

  /**
   * Get the output of the command (mixed stdout and stderr)
   * In case of error, contains error description
   */
  LPCTSTR getOutput();

  /**
   * Get the return value of the process. If the process is still
   * running, the return value is -1.
   */
  int getExitValue();


protected:
  /**
   * Initialize the process execution datastructure. Do not call while
   * running the process.
   */
  void initialize();
  /**
   * Start the process in the directory path. Make sure that the
   * executable is either in the path or specify the full path.
   * Capture stdin, stdout and stderr, and prepare for waiting
   * end of process to get exit code
   */
  BOOL startProcessCapture(LPCTSTR lpstrCommand, LPCTSTR lpstrPath);
  BOOL realPopenCreateProcess(LPCTSTR lpstrCommand, LPCTSTR lpstrPath);
  /**
   * Wait for the process to finish and get exit code and output
   */
  BOOL waitCapture();
  BOOL closeHandles();

  /**
   * Search in memory all processes listed in pProcesses array
   * or any new child process of an already listed process
   * Also remove for array non existing process
   **/
  BOOL parseRunningProcesses( CObArray *pProcessList);
  BOOL isProcessListed( CObArray *pProcessList, DWORD dwProcessID);

protected:
  HANDLE m_hProcessHandle;
  HANDLE m_hChildStdinRd;
  HANDLE m_hChildStdinWr;
  HANDLE m_hChildStdoutRd;
  HANDLE m_hChildStdoutWr;
  HANDLE m_hChildStderrRd;
  HANDLE m_hChildStderrWr;
  HANDLE m_hChildStdinWrDup;
  HANDLE m_hChildStdoutRdDup;
  HANDLE m_hChildStderrRdDup;
  DWORD  m_dwTimeout;
  BOOL	 m_bComspec;
  
  int m_fdStdIn;
  int m_fdStdOut;
  int m_fdStdErr;

  int m_nExitValue;

  CString m_csOutput;
  CString m_csConsoleSpawn;
};
