/*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*
| Hypnos UO Server Emulator                                                |
|                                                                          |
| This software is free software released under GPL2 license.              |
| You can find detailed license information in hypnos.cpp file.            |
|                                                                          |
*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*/
#include "common_libs.h"
#include "logsystem.h"
#include "extras/bounty.h"
#include "objects/cchar.h"
#include "objects/citem/cmsgboard.h"

/*
//////////////////////////////////////////////////////////////////////////////
// FUNCTION:    BountyAskVictim( int nVictimSerial, int nMurdererSerial )
//
// PURPOSE:     Used to ask the murder victim whether they would like to place
//              a bounty on the head of the murderer.
//
// PARAMETERS:  nVictimSerial   Victim characters serial number
//              nMurdererSerial Murderer characters serial number
//
// RETURNS:     void
//////////////////////////////////////////////////////////////////////////////
void BountyAskVictim( int nVictimSerial, int nMurdererSerial )
{
  int nAmount     = 0;
  int nVictimIdx  = calcCharFromSer( nVictimSerial );
  int nMurderIdx  = calcCharFromSer( nMurdererSerial );
  int err2;

  pChar pcc_nVictimIdx=MAKE_CHARREF_LOGGED(nVictimIdx,err);
  pChar pcc_nMurderIdx=MAKE_CHARREF_LOGGED(nMurderIdx,err2);

  if (err || err2) return;

  // Indicate that the victim is being asked if they want to place a bounty
  LogMessage("BountyAskVictim():  %s is attempting to place a bounty of %i on %s\n",
          pcc_nVictimIdx->getCurrentName().c_str(), nAmount, pcc_nMurderIdx->getCurrentName().c_str() );

  // If the amount of the bounty is larger than zero, create a bounty posting
  if( nAmount > 0 )
  {
     if( BountyCreate( pcc_nMurderIdx, nAmount ) )
	 {
		 LogMessage("BountyAskVictim():  %s has placed a bounty of %i on %s\n",
                pcc_nVictimIdx->getCurrentName().c_str(),
                nAmount,
                pcc_nMurderIdx->getCurrentName().c_str() );
	 }

     else
	 {
	   LogMessage("BountyAskVictim():  %s FAILED to place a bounty of %i on %s\n",
               pcc_nVictimIdx->getCurrentName().c_str(),
               nAmount,
               pcc_nMurderIdx->getCurrentName().c_str() );
	 }
  }

  return;
} // BountyAskVictim()
*/

//////////////////////////////////////////////////////////////////////////////
// FUNCTION:    BountyCreate( int nMurdererSerial, int nRewardAmount )
//
// PURPOSE:     Used to create the bounty posting message on the global
//              bulletin board for all to see.
//
// PARAMETERS:  nMurdererSerial Murderer characters serial number
//              nRewardAmount   Bounty placed on the murderers head
//
// RETURNS:     TRUE  Bounty post successfully created
//              FALSE Bounty post could not be created
//////////////////////////////////////////////////////////////////////////////
bool BountyCreate( pChar pc, int nRewardAmount )
{
	if ( ! pc ) return false;

  int   nPostSerial = 0;

  // Check that we have a reward amount greater than zero
  if( nRewardAmount > 0 )
  {
    // Check that this murderer doesn't already have a bounty on them
    if( pc->questBountyReward > 0 )
    {
      // This murderer already has a bounty on them because they
      // have a reward amount on their head, so delete old bounty
      // and add the new (updated) one
      nRewardAmount += pc->questBountyReward;
      BountyDelete( pc );
    }

    // Attempt to post the message first
    pc->questBountyReward = nRewardAmount;
    nPostSerial = cMsgBoard::createQuestMessage( cMsgBoard::BOUNTYQUEST, pc->getSerial());

    // If we received a valid serial number then the post was successfull
    if( nPostSerial > 0 )
    {
      pc->questBountyPostSerial = nPostSerial;
      return true;
    }
  }

  // Failed to post bounty
  LogWarning("BountyCreate():  FAILED to place a bounty of %i on %s (PostSerial=%x)\n",
          nRewardAmount,
          pc->getCurrentName().c_str(),
          nPostSerial );

  // Post must have failed
  return false;
} // BountyCreate()


bool BountyDelete( pChar pc)
{
	if ( ! pc ) return false;
	bool  bReturn = true;
	cMsgBoard::removeQuestMessage(questBountyPostSerial);

	// Reset all bounty values for this character
	pc->questBountyReward     = 0;
	pc->questBountyPostSerial = 0;

	return bReturn;
} // BountyDelete()


bool BountyWithdrawGold( pChar pVictim, int nAmount )
{
	int has = pVictim->countBankGold();
	if (has < nAmount)
		return false;

	pItem pBox = pVictim->GetBankBox();
	if (!pBox)
		return false;	// shouldn't happen coz it's needed in CountBankGold...

	pBox->DeleteAmount(nAmount,0x0EED);
	return true;
}

