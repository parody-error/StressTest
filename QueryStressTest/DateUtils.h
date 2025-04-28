#pragma once

/*
**  Convert a SYSTEMTIME time to a variant time (double) including the milliseconds
**
**  Note:   The valid ranges for are variant time from January 1, 100 to December 31, 9999
**  Note:   This is a lossless conversion.
**          However, converting a variant time to a SYSTEMTIME and back to a variant time
**          may yield a different variant time.  This is because the variant time has
**          more precision than the system time thus information may be lost during this conversion
**          Converting a system time to a variant time and back to system time will yield
**          the same system time.
**	Note:   The SYSTEMTIME may be changed. This change occurs in the Win API SystemTimeToVariantTime.
**  (Copied from ODUtil 05/02/2013 NREPKA)
*/
static bool SystemTimeToVariantTimeEx( SYSTEMTIME* pSystemTime, double* pVariantTime )

{
	bool Retval = false;

	// Make sure pointers are valid, milliseconds are valid and conversion succeeds
	if( pSystemTime && pVariantTime && pSystemTime->wMilliseconds < 1000 && SystemTimeToVariantTime( pSystemTime, pVariantTime ) )
	{
		// now add the milliseconds to the variant time (subtract or add depending on variant time < 0 )
		if( *pVariantTime < 0 )
			*pVariantTime -= pSystemTime->wMilliseconds * 1.15740740740740741e-8;
		else
			*pVariantTime += pSystemTime->wMilliseconds * 1.15740740740740741e-8;

		Retval = true;
	}

	return Retval;
}
