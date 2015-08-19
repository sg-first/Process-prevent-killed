Process prevent killed
=======================
ring3 Right
--------------------------------------
* Modify the program's process to the system process
* Principles and the code is simple, it can prevent the Task Manager to kill the process
* Valid only in XP.Prevent kill limited, can’t protective ice sword
* [Reference](http://blog.csdn.net/KeSummer/archive/2008/05/18/2455379.aspx)

Hook NtQuerySystemInformation
--------------------------------------------------------
* hook NtQuerySystemInformation to hidden processes(ring 3 level)
* it can prevent the Task Manager to kill the process
* Valid only in NT2000. Not universal

Detours lib prevent killed
--------------------------------------------------------
* Principle is similar with HookNtQuerySystemInformation,hook OpenProcess to prevent kille.
* Valid in NT2000,xp and NT2003
* use detourslib,can't prevent some processes tools

Hook Taskmgr
--------------------------------------------------------
* Hook Taskmgr,use CBT hook block end process message.
* Valid in and windows version(As of 2009)
* Only for the Taskmgr

Dual-process protection
--------------------------------------------------------
* Two processes monitoring each other, found another prevent killed, start it.
* Versatile, protection capability, can protect Windows service program.
* Not really prevent killed, just rebooted.

Driver protection
--------------------------------------------------------
* use drive’s ZwQuerySystemInformation hook,to do hide or prevent killed process.
* Strong ability, can protective ice sword
* Complex, right to drive will be identified as Trojan by anti-virus software