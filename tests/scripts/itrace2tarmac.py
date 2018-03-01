#!/usr/bin/env python
#
# ----------------------------------------------------------------------------
# 
# itrace2tarmac.py
# 
# Copyright (C) 2015 by Freescale Semiconductor Inc.  All rights reserved.
# 
# Author: Lu Hao (B48275)
#
# Functionality:
#       Converts itrace log file to TARMAC trace format.
#
# Supported itrace format:
#       itrace.[Core#].zen @ [timestamp]: [instr_address] [opcode] [reg/mem update]
#
# Usage: 
#       ./itrace2tarmac.py [itrace_file_name] [number_of_cores] [(optional) output_file_name]
#
# Example 1 (export all 3 cores): 
#       ./itrace2tarmac.py alivevcsmx_VPD_sim.log 3
# 
# Example 2 (export core 0 only): 
#       ./itrace2tarmac.py alivevcsmx_VPD_sim.log 1 alivevcs_tarmac.tarmac
#
# Parsing rules:
#       1. Instruction: always starts from the first right after timestamp and ":", 
#           format: [address] [opcode]
#       2. Register: flexible position, fixed format: [reg_name=value]
#       3. Memory access: flexible position, fixed format: 
#           read - [r(address)] [data]; write - [w(address)] [data]
#       4. Exception: Marked as #fault#
# ----------------------------------------------------------------------------

#############################################################################################
#                                global functions                                           #
#############################################################################################
    
def read_file(file_name):
    # try:
        # eval("with open(file_name,'r') as file_object: ")
    # except SyntaxError:
        # raise ImportError("Current Python version is not supported.")
        # exit(1)
    try:
        with open(file_name,'r') as file_object: 
            all_lines = file_object.readlines()    
    except IOError as e:
        print '\n[ERROR] Unable to open file "' + file_name + '". Please make sure the file exists and have access permission.\n'
        exit(1)   
    
    return all_lines
    
def write_file(file_lines, file_name):
    try:
        with open(file_name,'w') as file_object: 
            file_object.writelines(file_lines)
    except IOError as e:
        slash_pos = file_name.rfind('/')
        if slash_pos != -1:
            print '\n[ERROR] Unable to open file "' + file_name + '". Please create the directory "' + file_name[:slash_pos] + '" first.\n'
        else:
            print '\n[ERROR] Unable to open file "' + file_name + '". May because of permission denied.\n'
        exit(1)   

#############################################################################################
#                                         main                                              #
#############################################################################################
LINE_START = ['branch.', 'itrace.', 'htrace.', 'etrace.', 'spr_trace']
EXCEPTION_TAG = '#fault#'
ITRACE_SEPARATOR = ':'
ITRACE_SEPARATOR_timestamp = '@'
ITRACE_SEPARATOR_core = '.'


if __name__ == '__main__':        
    
    import os, sys
    version = sys.version_info
    # print version
    if version < (2, 7):
        print '[ERROR] Python version ' + version + ' is not supported. Please use python version 2.7.'    
        exit(1)
    else:
        pass
        
    itrace_name = ""
    if len(sys.argv) == 3 or len(sys.argv) == 4:
        pass
    else:
        print "\n[ERROR] Incorrect number of arguments.\n\nUsage: {} [itrace_file_name] [total_number_of_cores] [(optional) output_file_name]".format(os.path.basename(sys.argv[0]))
        exit(1)
        
    # Read itrace file
    itrace_name = sys.argv[1]
    name_dot_pos = itrace_name.rfind('.')
    if name_dot_pos != -1:
        if itrace_name[name_dot_pos+1] != '/': # a valid dot position
            pass
        else:
            name_dot_pos = -1 # still have no extension, reset this position to -1
    if name_dot_pos == -1:
        name_dot_pos = len(itrace_name)
        
    try:
        N_cores = int(sys.argv[2])
    except ValueError:
        print "\n[ERROR] Total number of cores should be a number.\n\nUsage: {} [itrace_file_name] [total_number_of_cores] [(optional) output_file_name]".format(os.path.basename(sys.argv[0]))
        exit(1)
    lines_itrace = read_file(itrace_name)

    print 'Converting ...'
    tarmac_files = []    
    for i in range(N_cores):
        tarmac_files.append([])
        tarmac_files[i].append('# This file is converted from itrace file: ' + itrace_name + '\n')
        tarmac_files[i].append('# Core: ' + str(i) + '\n')
    
    Timestamp = '0ns'
    # Process each line
    for line in lines_itrace:
        if any([line.startswith(s) for s in LINE_START]):
            # print line
            IT_Addr = ''
            IT_Data = ''
            MW_Addr = ''
            MW_Data = ''
            MR_Addr = ''
            MR_Data = ''
            R_Name = ''
            R_Data = ''
            msg = ''
            skipLine = False

            line_secs = line.split(ITRACE_SEPARATOR, 1)
            # print line_secs
            line_secs_0 = line_secs[0].split(ITRACE_SEPARATOR_timestamp)
            # print line_secs_0
            line_secs_0_0 = line_secs_0[0].split(ITRACE_SEPARATOR_core)
            try:
                core_number = int(line_secs_0_0[1])
            except ValueError:
                print '[ERROR] unrecognized itrace format: ' + line
            
            # record timestamp and instruction
            try:
                Timestamp = line_secs_0[1].strip(' ')
            except IndexError:
                # print '[WARNING] Cannot find timestamp info in line:\n    ' + line
                # exit(1)
                pass
            try:
                line_secs_1 = line_secs[1].split()
            except IndexError:
                print line
            if len(line_secs_1) > 1:
                try: 
                    int(line_secs_1[0],16)
                    int(line_secs_1[1],16)     
                    IT_Addr = line_secs_1[0]
                    IT_Data = line_secs_1[1]
                    msg = '   ' + Timestamp + ' MW4_I ' + IT_Addr + ' ' + IT_Data + '\n'                    
                    msg += '   ' + Timestamp + ' IT ' + IT_Addr + ' ' + IT_Data + ' opcode data ' + '\n'                    
                except ValueError: # 2 or more secs irregular msg
                    if line_secs_1[0] == '+':
                        msg = ''
                        # msg = '   # itrace msg: (appending value)\n'
                        pass
                    else:
                        msg = '' 
                        # msg = '   # itrace msg: ' + line  
            else:  # 1 secs irregular msg
                e_pos = line_secs[1].find('=')
                if e_pos != -1:
                    try:
                        int(line_secs[1][e_pos+1:], 16) # if the thing after "=" is a valid hex
                    except ValueError:
                        msg = '   # (Untranslated format): ' + Timestamp + ' ' + line_secs[1] + '\n' 
                elif line_secs[1] == EXCEPTION_TAG:
                    msg = '   ' + Timestamp + ' E ' + line_secs[1] + '\n' 
                else:
                    msg = '' 
                    # msg = '   # (Untranslated format): ' + Timestamp + ' ' + line_secs[1] + '\n' 
            try:
                tarmac_files[core_number].append(msg)
            except IndexError:
                print '\n[WARNING] Parsing core ' + str(core_number) + ' is disabled. (You entered total number of cores is: ' + str(N_cores) + ')\n'
                continue
                # print '\n[ERROR] You entered total number of cores "' + str(N_cores) + '" is incorrect. now reading core: ' + str(core_number) + '\n'
                # exit(1)
                # else:
                    # pass
            
            # if skipLine:
                # continue
            # record reg/mem update
            info = ''
            for i in range(len(line_secs_1)):
                # print line_secs_1[i] 
                try:
                    e_pos = line_secs_1[i].find('=')
                    # print e_pos
                    # Register change
                    if e_pos != -1:
                        if line_secs_1[i].startswith(str(core_number)):
                            b_pos = line_secs_1[i].find('[')
                            if b_pos != -1: # i.e. 0r0[32:63]=00000000
                                R_Name = line_secs_1[i][1:b_pos] + ' ' + line_secs_1[i][b_pos:e_pos]
                            else: # i.e. 0ctr=00000000
                                R_Name = line_secs_1[i][1:e_pos]
                        else:
                            R_Name = line_secs_1[i][:e_pos]
                        R_Data = line_secs_1[i][e_pos+1:]
                        msg = '   ' + Timestamp + ' R ' + R_Name + ' ' + R_Data + '\n'
                    # memory write
                    elif line_secs_1[i].startswith('w'):
                        MW_Addr = line_secs_1[i][2:len(line_secs_1[i])-1]
                        MW_Data = line_secs_1[i+1][line_secs_1[i+1].find('=')+1:]
                        msg = '   ' + Timestamp + ' MW4_D ' + MW_Addr + ' ' + MW_Data + '\n'
                    # memory read
                    elif line_secs_1[i].startswith('r'):
                        MR_Addr = line_secs_1[i][2:len(line_secs_1[i])-1]
                        MR_Data = line_secs_1[i+1][line_secs_1[i+1].find('=')+1:]
                        msg = '   ' + Timestamp + ' MR4_D ' + MR_Addr + ' ' + MR_Data + '\n'
                    elif line_secs_1[i] == EXCEPTION_TAG:
                        msg = '   ' + Timestamp + ' E ' + line_secs_1[i] + '\n' 
                    else:
                        try:
                            int(line_secs_1[i],16)
                        except ValueError:
                            if line_secs_1[i] != '+':
                                info = info + line_secs_1[i] + ' '
                            else:
                                pass
                        continue # don't do "append msg"
                        
                    tarmac_files[core_number].append(msg)
                except IndexError:
                    print 'Unknown format line: ' + line
                    pass
            if info != '':
                tarmac_files[core_number].append('   ' + Timestamp + ' E ' + info + '\n')
                
    for i in range(N_cores):
        output_file = ''
        try:
            dot_pos = sys.argv[3].rfind('.')
            if dot_pos == -1 or sys.argv[3][dot_pos-1] == '.': # for ../../aaa case
                output_file = sys.argv[3] + '_Core_' + str(i) + '.tarmac'
            else:
                output_file = sys.argv[3][:dot_pos] + '_Core_' + str(i) + sys.argv[3][dot_pos:]
        except IndexError:   # use default output file names
            itrace_basename = os.path.basename(itrace_name[:name_dot_pos])
            output_file = 'tarmac_' + itrace_basename + '_Core_' + str(i) + '.tarmac'
                
        write_file(tarmac_files[i], output_file)
        print '\n[INFO] Conversion finished. Output file: ' + output_file
        if len(tarmac_files[i]) == 2:
            print '[WARNING] Empty TARMAC file for Core ' + str(i) 

    
