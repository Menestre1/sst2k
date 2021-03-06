#include "sst.h"
#include <string.h>
#include <time.h>

void selfdestruct(void)
/* self-destruct maneuver */
{
    /* Finish with a BANG! */
    chew();
    if (damaged(DCOMPTR)) {
	prout(_("Computer damaged; cannot execute destruct sequence."));
	return;
    }
    prouts(_("---WORKING---")); skip(1);
    prouts(_("SELF-DESTRUCT-SEQUENCE-ACTIVATED")); skip(1);
    prouts("   10"); skip(1);
    prouts("       9"); skip(1);
    prouts("          8"); skip(1);
    prouts("             7"); skip(1);
    prouts("                6"); skip(1);
    skip(1);
    prout(_("ENTER-CORRECT-PASSWORD-TO-CONTINUE-"));
    skip(1);
    prout(_("SELF-DESTRUCT-SEQUENCE-OTHERWISE-"));
    skip(1);
    prout(_("SELF-DESTRUCT-SEQUENCE-WILL-BE-ABORTED"));
    skip(1);
    scan();
    chew();
    if (strcmp(game.passwd, citem) != 0) {
	prouts(_("PASSWORD-REJECTED;"));
	skip(1);
	prouts(_("CONTINUITY-EFFECTED"));
	skip(2);
	return;
    }
    prouts(_("PASSWORD-ACCEPTED")); skip(1);
    prouts("                   5"); skip(1);
    prouts("                      4"); skip(1);
    prouts("                         3"); skip(1);
    prouts("                            2"); skip(1);
    prouts("                              1"); skip(1);
    if (Rand() < 0.15) {
	prouts(_("GOODBYE-CRUEL-WORLD"));
	skip(1);
    }
    kaboom();
}

void kaboom(void) 
{
    stars();
    if (game.ship==IHE)
	prouts("***");
    prouts(_("********* Entropy of "));
    crmshp();
    prouts(_(" maximized *********"));
    skip(1);
    stars();
    skip(1);
    if (game.nenhere != 0) {
	double whammo = 25.0 * game.energy;
	int l=1;
	while (l <= game.nenhere) {
	    if (game.kpower[l]*game.kdist[l] <= whammo) 
		deadkl(game.ks[l], game.quad[game.ks[l].x][game.ks[l].y], game.ks[l]);
	    l++;
	}
    }
    finish(FDILITHIUM);
}
				

void finish(FINTYPE ifin)
/* end the game, with appropriate notfications */
{
    bool igotit = false;
    game.alldone = true;
    skip(3);
    prout(_("It is stardate %.1f."), game.state.date);
    skip(1);
    switch (ifin) {
    case FWON: // Game has been won
	if (game.state.nromrem != 0)
	    prout(_("The remaining %d Romulans surrender to Starfleet Command."),
		  game.state.nromrem);

	prout(_("You have smashed the Klingon invasion fleet and saved"));
	prout(_("the Federation."));
	game.gamewon = true;
	if (game.alive) {
	    double badpt;
	    badpt = 5.0*game.state.starkl \
		+ game.casual \
		+ 10.0*game.state.nplankl \
		+ 300.0*game.state.nworldkl \
		+ 45.0*game.nhelp \
		+ 100.0*game.state.basekl \
		+ 3.0*game.abandoned;
	    if (game.ship == IHF)
		badpt += 100.0;
	    else if (game.ship == 0)
		badpt += 200.0;
	    if (badpt < 100.0)
		badpt = 0.0;	// Close enough!
	    if (game.state.date-game.indate < 5.0 ||
		// killsPerDate >= RateMax
		((game.inkling + game.incom + game.inscom) - (game.state.remkl + game.state.remcom + game.state.nscrem))/(game.state.date-game.indate) >=
		0.1*game.skill*(game.skill+1.0) + 0.1 + 0.008*badpt) {
		skip(1);
		prout(_("In fact, you have done so well that Starfleet Command"));
		switch (game.skill) {
		case SKILL_NONE:
		    break;	/* quiet gcc warning */
		case SKILL_NOVICE:
		    prout(_("promotes you one step in rank from \"Novice\" to \"Fair\"."));
		    break;
		case SKILL_FAIR:
		    prout(_("promotes you one step in rank from \"Fair\" to \"Good\"."));
		    break;
		case SKILL_GOOD:
		    prout(_("promotes you one step in rank from \"Good\" to \"Expert\"."));
		    break;
		case SKILL_EXPERT:
		    prout(_("promotes you to Commodore Emeritus."));
		    skip(1);
		    prout(_("Now that you think you're really good, try playing"));
		    prout(_("the \"Emeritus\" game. It will splatter your ego."));
		    break;
		case SKILL_EMERITUS:
		    skip(1);
		    proutn(_("Computer-  "));
		    prouts(_("ERROR-ERROR-ERROR-ERROR"));
		    skip(2);
		    prouts(_("  YOUR-SKILL-HAS-EXCEEDED-THE-CAPACITY-OF-THIS-PROGRAM"));
		    skip(1);
		    prouts(_("  THIS-PROGRAM-MUST-SURVIVE"));
		    skip(1);
		    prouts(_("  THIS-PROGRAM-MUST-SURVIVE"));
		    skip(1);
		    prouts(_("  THIS-PROGRAM-MUST-SURVIVE"));
		    skip(1);
		    prouts(_("  THIS-PROGRAM-MUST?- MUST ? - SUR? ? -?  VI"));
		    skip(2);
		    prout(_("Now you can retire and write your own Star Trek game!"));
		    skip(1);
		    break;
		}
		if (game.skill >= SKILL_EXPERT) {
		    if (game.thawed && !idebug)
			prout(_("You cannot get a citation, so..."));
		    else {
			proutn(_("Do you want your Commodore Emeritus Citation printed? "));
			chew();
			if (ja() == true) {
			    igotit = true;
			}
		    }
		}
	    }
	    // Only grant long life if alive (original didn't!)
	    skip(1);
	    prout(_("LIVE LONG AND PROSPER."));
	}
	score();
	if (igotit)
	    plaque();	    
	return;
    case FDEPLETE: // Federation Resources Depleted
	prout(_("Your time has run out and the Federation has been"));
	prout(_("conquered.  Your starship is now Klingon property,"));
	prout(_("and you are put on trial as a war criminal.  On the"));
	proutn(_("basis of your record, you are "));
	if ((game.state.remkl + game.state.remcom + game.state.nscrem)*3.0 > (game.inkling + game.incom + game.inscom)) {
	    prout(_("acquitted."));
	    skip(1);
	    prout(_("LIVE LONG AND PROSPER."));
	}
	else {
	    prout(_("found guilty and"));
	    prout(_("sentenced to death by slow torture."));
	    game.alive = false;
	}
	score();
	return;
    case FLIFESUP:
	prout(_("Your life support reserves have run out, and"));
	prout(_("you die of thirst, starvation, and asphyxiation."));
	prout(_("Your starship is a derelict in space."));
	break;
    case FNRG:
	prout(_("Your energy supply is exhausted."));
	skip(1);
	prout(_("Your starship is a derelict in space."));
	break;
    case FBATTLE:
	proutn(_("The "));
	crmshp();
	prout(_("has been destroyed in battle."));
	skip(1);
	prout(_("Dulce et decorum est pro patria mori."));
	break;
    case FNEG3:
	prout(_("You have made three attempts to cross the negative energy"));
	prout(_("barrier which surrounds the galaxy."));
	skip(1);
	prout(_("Your navigation is abominable."));
	score();
	return;
    case FNOVA:
	prout(_("Your starship has been destroyed by a nova."));
	prout(_("That was a great shot."));
	skip(1);
	break;
    case FSNOVAED:
	proutn(_("The "));
	crmshp();
	prout(_(" has been fried by a supernova."));
	prout(_("...Not even cinders remain..."));
	break;
    case FABANDN:
	prout(_("You have been captured by the Klingons. If you still"));
	prout(_("had a starbase to be returned to, you would have been"));
	prout(_("repatriated and given another chance. Since you have"));
	prout(_("no starbases, you will be mercilessly tortured to death."));
	break;
    case FDILITHIUM:
	prout(_("Your starship is now an expanding cloud of subatomic particles"));
	break;
    case FMATERIALIZE:
	prout(_("Starbase was unable to re-materialize your starship."));
	prout(_("Sic transit gloria mundi"));
	break;
    case FPHASER:
	proutn(_("The "));
	crmshp();
	prout(_(" has been cremated by its own phasers."));
	break;
    case FLOST:
	prout(_("You and your landing party have been"));
	prout(_("converted to energy, disipating through space."));
	break;
    case FMINING:
	prout(_("You are left with your landing party on"));
	prout(_("a wild jungle planet inhabited by primitive cannibals."));
	skip(1);
	prout(_("They are very fond of \"Captain Kirk\" soup."));
	skip(1);
	proutn(_("Without your leadership, the "));
	crmshp();
	prout(_(" is destroyed."));
	break;
    case FDPLANET:
	prout(_("You and your mining party perish."));
	skip(1);
	prout(_("That was a great shot."));
	skip(1);
	break;
    case FSSC:
	prout(_("The Galileo is instantly annihilated by the supernova."));
	// no break;
    case FPNOVA:
	prout(_("You and your mining party are atomized."));
	skip(1);
	proutn(_("Mr. Spock takes command of the "));
	crmshp();
	prout(_(" and"));
	prout(_("joins the Romulans, reigning terror on the Federation."));
	break;
    case FSTRACTOR:
	prout(_("The shuttle craft Galileo is also caught,"));
	prout(_("and breaks up under the strain."));
	skip(1);
	prout(_("Your debris is scattered for millions of miles."));
	proutn(_("Without your leadership, the "));
	crmshp();
	prout(_(" is destroyed."));
	break;
    case FDRAY:
	prout(_("The mutants attack and kill Spock."));
	prout(_("Your ship is captured by Klingons, and"));
	prout(_("your crew is put on display in a Klingon zoo."));
	break;
    case FTRIBBLE:
	prout(_("Tribbles consume all remaining water,"));
	prout(_("food, and oxygen on your ship."));
	skip(1);
	prout(_("You die of thirst, starvation, and asphyxiation."));
	prout(_("Your starship is a derelict in space."));
	break;
    case FHOLE:
	prout(_("Your ship is drawn to the center of the black hole."));
	prout(_("You are crushed into extremely dense matter."));
	break;
    case FCREW:
	prout(_("Your last crew member has died."));
	break;
    }
    if (game.ship==IHF)
	game.ship = IHBLANK;
    else if (game.ship == IHE)
	game.ship = IHF;
    game.alive = false;
    if ((game.state.remkl + game.state.remcom + game.state.nscrem) != 0) {
	double goodies = game.state.remres/game.inresor;
	double baddies = (game.state.remkl + 2.0*game.state.remcom)/(game.inkling+2.0*game.incom);
	if (goodies/baddies >= 1.0+0.5*Rand()) {
	    prout(_("As a result of your actions, a treaty with the Klingon"));
	    prout(_("Empire has been signed. The terms of the treaty are"));
	    if (goodies/baddies >= 3.0+Rand()) {
		prout(_("favorable to the Federation."));
		skip(1);
		prout(_("Congratulations!"));
	    }
	    else
		prout(_("highly unfavorable to the Federation."));
	}
	else
	    prout(_("The Federation will be destroyed."));
    }
    else {
	prout(_("Since you took the last Klingon with you, you are a"));
	prout(_("martyr and a hero. Someday maybe they'll erect a"));
	prout(_("statue in your memory. Rest in peace, and try not"));
	prout(_("to think about pigeons."));
	game.gamewon = true;
    }
    score();
}

static void score_item(const char *str, int num, int score)
{
    if (num) {
	prout(str, num, score);
	iscore += score;
    }
}

static void score_item1(const char *str, int score)
{
    prout(str, score);
    iscore += score;
}

static void score_itemf(const char *str, float num, int score)
{
    if (num > 0) {
	prout(str, num, score);
	iscore += score;
    }
}

void score(void) 
/* compute player's score */
{
    double timused = game.state.date - game.indate;
    int iwon, klship, num;

    iskill = game.skill;
    if ((timused == 0 || (game.state.remkl + game.state.remcom + game.state.nscrem) != 0) && timused < 5.0)
	timused = 5.0;
    iwon = 0;
    if (game.gamewon)
	iwon = 100*game.skill;
    if (game.ship == IHE) 
	klship = 0;
    else if (game.ship == IHF) 
	klship = 1;
    else
	klship = 2;
    iscore = 0;
    skip(2);
    prout(_("Your score --"));
    num = game.inrom - game.state.nromrem;
    score_item(_("%6d Romulans destroyed                 %5d"), num, 20 * num);
    if (game.gamewon)
	score_item(_("%6d Romulans captured                  %5d"),
	      game.state.nromrem, game.state.nromrem);
    num = game.inkling - game.state.remkl;
    score_item(_("%6d ordinary Klingons destroyed        %5d"), num, 10 * num);
    num = game.incom - game.state.remcom;
    score_item(_("%6d Klingon commanders destroyed       %5d"), num, 50 * num);
    num = game.inscom - game.state.nscrem;
    score_item(_("%6d Super-Commander destroyed          %5d"), num, 200 * num);
    perdate = ((game.inkling + game.incom + game.inscom) -
	    (game.state.remkl + game.state.remcom + game.state.nscrem))/timused;
    score_itemf(_("%6.2f Klingons per stardate              %5d"), perdate,
	    500 * perdate + 0.5);
    score_item(_("%6d stars destroyed by your action     %5d"),
	    game.state.starkl, -5*game.state.starkl);
    score_item(_("%6d planets destroyed by your action   %5d"),
	    game.state.nplankl, -10*game.state.nplankl);
    if (game.options & OPTION_WORLDS)
	score_item(_("%6d inhabited planets destroyed by your action   %5d"),
		game.state.nworldkl, -300*game.state.nworldkl);
    score_item(_("%6d bases destroyed by your action     %5d"),
	    game.state.basekl, -100*game.state.basekl);
    score_item(_("%6d calls for help from starbase       %5d"),
	    game.nhelp, -45*game.nhelp);
    score_item(_("%6d casualties incurred                %5d"),
	    game.casual, -game.casual);
    score_item(_("%6d crew abandoned in space            %5d"),
	    game.abandoned, -3*game.abandoned);
    score_item(_("%6d ship(s) lost or destroyed          %5d"),
	    klship, -100*klship);
    if (!game.alive)
	score_item1(_("Penalty for getting yourself killed        %5d"), -200);
    if (game.gamewon) {
	proutn(_("Bonus for winning "));
	switch (game.skill) {
	case SKILL_NONE:     break;	/* quiet gcc warning */
	case SKILL_NOVICE:   proutn(_("Novice game  ")); break;
	case SKILL_FAIR:     proutn(_("Fair game    ")); break;
	case SKILL_GOOD:     proutn(_("Good game    ")); break;
	case SKILL_EXPERT:   proutn(_("Expert game  ")); break;
	case SKILL_EMERITUS: proutn(_("Emeritus game")); break;
	}
	prout("           %5d", iwon);
    }
    skip(1);
    prout(_("TOTAL SCORE                               %5d"), iscore);
}

void plaque(void)
/* emit winner's commemmorative plaque */
{
	FILE *fp=NULL;
	time_t t;
	char *timestring;
	int nskip;
	char winner[128];

	skip(2);
	while (fp == NULL) {
                proutn(_("File or device name for your plaque: "));
                cgetline(winner, sizeof(winner));
		fp = fopen(winner, "w");
		if (fp==NULL) {
                        prout(_("Invalid name."));
		}
	}

        proutn(_("Enter name to go on plaque (up to 30 characters): "));
        cgetline(winner, sizeof(winner));
	/* The 38 below must be 64 for 132-column paper */
	nskip = 38 - strlen(winner)/2;

	fprintf(fp,"\n\n\n\n");
	/* --------DRAW ENTERPRISE PICTURE. */
        fprintf(fp, "                                       EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n" );
	fprintf(fp, "                                      EEE                      E  : :                                         :  E\n" );
	fprintf(fp, "                                    EE   EEE                   E  : :                   NCC-1701              :  E\n");
        fprintf(fp, "EEEEEEEEEEEEEEEE        EEEEEEEEEEEEEEE  : :                              : E\n");
        fprintf(fp, " E                                     EEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE\n");
	fprintf(fp, "                      EEEEEEEEE               EEEEEEEEEEEEE                 E  E\n");
	fprintf(fp, "                               EEEEEEE   EEEEE    E          E              E  E\n");
	fprintf(fp, "                                      EEE           E          E            E  E\n");
	fprintf(fp, "                                                       E         E          E  E\n");
	fprintf(fp, "                                                         EEEEEEEEEEEEE      E  E\n");
	fprintf(fp, "                                                      EEE :           EEEEEEE  EEEEEEEE\n");
	fprintf(fp, "                                                    :E    :                 EEEE       E\n");
	fprintf(fp, "                                                   .-E   -:-----                       E\n");
	fprintf(fp, "                                                    :E    :                            E\n");
	fprintf(fp, "                                                      EE  :                    EEEEEEEE\n");
	fprintf(fp, "                                                       EEEEEEEEEEEEEEEEEEEEEEE\n");
	fprintf(fp, "\n\n\n");
	fprintf(fp, _("                                                       U. S. S. ENTERPRISE\n"));
	fprintf(fp, "\n\n\n\n");
	fprintf(fp, _("                                  For demonstrating outstanding ability as a starship captain\n"));
	fprintf(fp, "\n");
	fprintf(fp, _("                                                Starfleet Command bestows to you\n"));
	fprintf(fp, "\n");
	fprintf(fp,"%*s%s\n\n", nskip, "", winner);
	fprintf(fp, _("                                                           the rank of\n\n"));
	fprintf(fp, _("                                                       \"Commodore Emeritus\"\n\n"));
	fprintf(fp, "                                                          ");
	switch (iskill) {
		case SKILL_EXPERT: fprintf(fp,_(" Expert level\n\n")); break;
		case SKILL_EMERITUS: fprintf(fp,_("Emeritus level\n\n")); break;
		default: fprintf(fp,_(" Cheat level\n\n")); break;
	}
	t = time(NULL);
	timestring = ctime(&t);
	fprintf(fp, _("                                                 This day of %.6s %.4s, %.8s\n\n"),
			timestring+4, timestring+20, timestring+11);
	fprintf(fp,_("                                                        Your score:  %d\n\n"), iscore);
	fprintf(fp,_("                                                    Klingons per stardate:  %.2f\n"), perdate);
	fclose(fp);
}
