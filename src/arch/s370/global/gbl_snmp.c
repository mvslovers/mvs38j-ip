/* gbl_snmp.c - define SNMP global data area */
// Defines whatever h/snmpvars.h extern's - jmm

/* System & Interface MIB */

char		SysDescr[256], SysContact[256], SysName[256], SysLocation[256];
unsigned	SysUpTime, SysServices, IfNumber;

/* IP MIB */
unsigned
	IpForwarding, IpDefaultTTL, IpInReceives, IpInHdrErrors,
	IpInAddrErrors, IpForwDatagrams, IpInUnknownProtos, IpInDiscards,
	IpInDelivers, IpOutRequests, IpOutDiscards, IpOutNoRoutes,
	IpReasmTimeout, IpReasmReqds, IpReasmOKs, IpReasmFails, IpFragOKs,
	IpFragFails, IpFragCreates, IpRoutingDiscards;

/* ICMP MIB */
unsigned
	IcmpInMsgs, IcmpInErrors, IcmpInDestUnreachs, IcmpInTimeExcds,
	IcmpInParmProbs, IcmpInSrcQuenchs, IcmpInRedirects, IcmpInEchos,
	IcmpInEchoReps, IcmpInTimestamps, IcmpInTimestampReps,
	IcmpInAddrMasks, IcmpInAddrMaskReps, IcmpOutMsgs, IcmpOutErrors,
	IcmpOutDestUnreachs, IcmpOutTimeExcds, IcmpOutParmProbs,
	IcmpOutSrcQuenchs, IcmpOutRedirects, IcmpOutEchos,
	IcmpOutEchoReps, IcmpOutTimestamps, IcmpOutTimestampReps,
	IcmpOutAddrMasks, IcmpOutAddrMaskReps;

/* UDP MIB */
unsigned
	UdpInDatagrams, UdpNoPorts, UdpInErrors, UdpOutDatagrams;

/* TCP MIB */
unsigned
	TcpRtoAlgorithm, TcpRtoMin, TcpRtoMax, TcpMaxConn, TcpActiveOpens,
	TcpPassiveOpens, TcpAttemptFails, TcpEstabResets, TcpCurrEstab,
	TcpInSegs, TcpOutSegs, TcpRetransSegs, TcpInErrs, TcpOutRsts;

/* SNMP MIB */
unsigned
        snmpInPkts, snmpOutPkts, snmpInBadVersions,
        snmpInBadCommunityNames, snmpInBadCommunityUses,
        snmpInASNParseErrs, snmpInTooBigs, snmpInNoSuchNames,
        snmpInBadValues, snmpInReadOnlys, snmpInGenErrs,
        snmpInTotalReqVars, snmpInTotalSetVars, snmpInGetRequests,
        snmpInGetNexts, snmpInSetRequests, snmpInGetResponses,
        snmpInTraps, snmpOutTooBigs, snmpOutNoSuchNames,
        snmpOutBadValues, snmpOutGenErrs, snmpOutGetRequests,
        snmpOutGetNexts, snmpOutSetRequests, snmpOutGetResponses,
        snmpOutTraps, snmpEnableAuthenTraps;



