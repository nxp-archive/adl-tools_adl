#ifndef __E_MESSAGE_PRIORITY_H_
#define __E_MESSAGE_PRIORITY_H_

namespace _asm_ {


enum e_message_priority
{
	// notes:
	// * in 'e_info', the user is noticed about:
	//   - details that are legal that may cause confusion or should be noted.
	//     example: 
	//        zz set 4
	//        ....
	//        zz set 8   --> the user may not noticed that 'zz' was defined (although this syntax is legal !).
	//     example: 
	//        "end" from macro or include file
	//     example: 
	//        zz set 4
	//     example: 
	//        break ttt	;no relevant loop
	//        nop
	//        end
	e_info,

	// notes:
	// * in 'e_note', the user is noticed about:
	//   - issues that are not documented.
	//     example:
	//        zz set 4 
	//        ....
	//        zz Gset 8   --> is 'zz' global or local??
	//     example:
	//       ttt macro XX2
	//         8%XX       --> the assembler treat % as modulo (and not as '%' of macros, since XX is no a macro-arg).
	//         endm
	e_note,

	// note:
	// * this enum must be ordered according the priority: info --> note --> warning --> ..
	//   to allow asking, for example: "if (priority <= e_warning)"
	e_warning,
	e_error,

	// note:
	// * 'e_severe_error' is used when the assembler cannot recover from the error, or it "can" but than many 
	//   irrelevant messages might be generated.
	// * 'e_severe_error' stops the assembler/mapper running, generates .lst-file and exit.
	//   Therefore, usually, there are no messages that refer to lines after 'e_severe_error' line.
	//   Exceptions: in the mapper, ino-messages that refer to aliases as 'inc' and 'clr' are generated before
	//   the mapping stage that may generate a severe-error (example: with mismatch with the ADL).
	e_severe_error,

	// used for 'msg' directive
	e_user_msg,

	e_num_of_message_priorities,
	e_no_such_message_priority
};


};// namespace

#endif
