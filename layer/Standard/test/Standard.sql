--#########################################
--# Standard unit test
--#########################################

--#########################################
--# INITIALIZE
--#########################################

--+MAKE testBacktrace FROM testBacktrace.c
--+MAKE testCM FROM testCM.c
--+MAKE testDigest FROM testDigest.c
-- +MAKE testEnv FROM testEnv.c
-- +MAKE testFile FROM testFile.c
--+MAKE testGetOpt FROM testGetOpt.c
--+MAKE testPoll FROM testPoll.c
-- +MAKE testProcNThread FROM testProcNThread.c
--+MAKE testSemaphoreAbnormalCase FROM testSemaphoreAbnormalCase.c
--+MAKE testSemaphoreNormalCase FROM testSemaphoreNormalCase.c
-- +MAKE testShm FROM testShm.c
-- +MAKE testShm2 FROM testShm2.c
--+MAKE testSignal FROM testSignal.c
--+MAKE testSocket FROM testSocket.c
-- +MAKE testStrings FROM testStrings.c
--+MAKE testTime FROM testTime.c
--+MAKE testUser FROM testUser.c

--#########################################
--# DO JOB
--#########################################

--+EXEC testBacktrace
--+EXEC testCM
--+EXEC testDigest
-- +EXEC testEnv
-- +EXEC testFile
--+EXEC testGetOpt
--+EXEC testPoll
-- +EXEC testProcNThread
--+EXEC testSemaphoreAbnormalCase
--+EXEC testSemaphoreNormalCase
-- +EXEC testShm
-- +EXEC testShm2
--+EXEC testSignal
--+EXEC testSocket
-- +EXEC testStrings
--+EXEC testTime
--+EXEC testUser

--#########################################
--# FINALIZE
--#########################################

--+REMOVE testBacktrace
--+REMOVE testCM
--+REMOVE testDigest
-- +REMOVE testEnv
-- +REMOVE testFile
--+REMOVE testGetOpt
--+REMOVE testPoll
-- +REMOVE testProcNThread
--+REMOVE testSemaphoreAbnormalCase
--+REMOVE testSemaphoreNormalCase
-- +REMOVE testShm
-- +REMOVE testShm2
--+REMOVE testSignal
--+REMOVE testSocket
-- +REMOVE testStrings
--+REMOVE testTime
--+REMOVE testUser

