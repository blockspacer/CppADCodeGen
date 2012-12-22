#ifndef CPPAD_CG_LINUX_SYSTEM_INCLUDED
#define CPPAD_CG_LINUX_SYSTEM_INCLUDED
/* --------------------------------------------------------------------------
 *  CppADCodeGen: C++ Algorithmic Differentiation with Source Code Generation:
 *    Copyright (C) 2012 Ciengis
 *
 *  CppADCodeGen is distributed under multiple licenses:
 *
 *   - Common Public License Version 1.0 (CPL1), and
 *   - GNU General Public License Version 2 (GPL2).
 *
 * CPL1 terms and conditions can be found in the file "epl-v10.txt", while
 * terms and conditions for the GPL2 can be found in the file "gpl2.txt".
 * ----------------------------------------------------------------------------
 * Author: Joao Leal
 */

#ifdef __linux__
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>

namespace CppAD {

    /**
     * Linux system dependent functions
     */
    namespace system {

        inline void createFolder(const std::string& folder) {
            int ret = mkdir(folder.c_str(), 0755);
            if (ret == -1) {
                if (errno != EEXIST) {
                    const char* error = strerror(errno);
                    CPPADCG_ASSERT_KNOWN(false, error);
                }
            }
        }

        inline std::string createPath(const std::string& baseFolder, const std::string& file) {
            return baseFolder + "/" + file;
        }

        inline std::string escapePath(const std::string& path) {
            return std::string("\"") + path + "\"";
        }

        inline std::string filenameFromPath(const std::string& path) {
            size_t pos = path.rfind('/');
            if (pos != std::string::npos) {
                if (pos == path.size() - 1) {
                    return "";
                } else {
                    return path.substr(pos + 1);
                }
            } else {
                return path;
            }
        }

        inline void callExecutable(const std::string& executable,
                                   const std::vector<std::string>& args,
                                   bool createPipe,
                                   const std::string& pipeMessage) {

            int fd[2];

            if (createPipe) {
                //Create pipe for piping source to the compiler
                CPPADCG_ASSERT_KNOWN(pipe(fd) == 0, "Failed to create pipe");
            }

            //Fork the compiler, pipe source to it, wait for the compiler to exit
            pid_t pid = fork();
            CPPADCG_ASSERT_KNOWN(pid >= 0, "Failed to fork program");

            if (pid == 0) {
                //  Child process

                if (createPipe) {
                    // close write end of pipe
                    close(fd[1]);
                    // Send pipe input to stdin
                    close(STDIN_FILENO);
                    dup2(fd[0], STDIN_FILENO);
                }

                std::vector<char*> args2(args.size() + 1);
                for (size_t i = 0; i < args.size(); i++) {
                    const size_t s = args[i].size() + 1;
                    args2[i] = new char[s];
                    for (size_t c = 0; c < s - 1; c++) {
                        args2[i][c] = args[i].at(c);
                    }
                    args2[i][s - 1] = '\0';
                }
                args2[args.size()] = (char *) NULL; // END             

                execv(executable.c_str(), &args2[0]);

                for (size_t i = 0; i < args.size(); i++) {
                    delete [] args2[i];
                }

                exit(0);
            }

            // Parent process
            if (createPipe) {
                // close read end of pipe
                close(fd[0]);
                //Pipe source to the executable
                write(fd[1], pipeMessage.c_str(), pipeMessage.size());
                close(fd[1]);
            }

            //Wait for the executable to exit
            int status;
            CPPADCG_ASSERT_KNOWN(wait(&status) >= 0, ("Failed while waiting for '" + executable + "'").c_str());
        }

        inline double currentTime() {
            struct timeval time;
            gettimeofday(&time, NULL);

            return time.tv_sec + ((double) time.tv_usec) / 1000000.0;
        }
    }
}
#endif

#endif
