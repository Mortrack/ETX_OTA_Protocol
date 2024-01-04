/**@file
 * 
 * @defgroup Terminal_Process Terminal Process module
 * @{
 * 
 * @brief   This module contains the
 *          {@link com.mortrack.hostbleapp.utils.TerminalProcess} Class, which
 *          provides the methods required so that the application can be able to
 *          to send terminal commands to the Operative System into which that
 *          Class is being run at, and to also receive responses from it.
 * 
 * @details The following gives a friendly code example of how to use
 *          {@link com.mortrack.hostbleapp.utils.TerminalProcess} Class:
 * @code
    // Test code for running a C compiled file from Java and to get its return value via the Terminal Window Stream.
    TerminalProcess terminalProcess = new TerminalProcess();
    String ret; // Will hold the response from the methods to be called.
    int timeout = 86400; // Giving 24 hours of timeout as an example.
    try {
        if (terminalProcess.isWindows()) {
            ret = terminalProcess.runExecutableFile(".\\APIs\\main.o 1 2 3", timeout); // For Windows users list files
        } else {
            ret = terminalProcess.runExecutableFile("./APIs/main.o 1 2 3", timeout); // for Linux based OS users list files
        }
        System.out.println(ret.equals("0")); // Checking if the response received matches the 0 ASCII Character.
    } catch (Exception ex) {
        Logger.getLogger(MainClass.class.getName()).log(Level.SEVERE, null, ex);
    }
 * @endcode
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @author  <a href=https://gist.github.com/Suranchiyev>Beknazar Suranchiyev</a>
 * @date    November 01, 2023.
 */


/* @brief   Package containing the class, methods, variables and code in
 *          general to provide the application with utility functionalities.
 *
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    November 01, 2023.
 */
package com.mortrack.hostbleapp.utils;

/* Imports used for this module. */
import java.io.*;
import java.util.concurrent.TimeUnit;

/**@brief   {@link com.mortrack.hostbleapp.utils.TerminalProcess} Class that
 *          provides several methods to enable the application with the
 *          capability of sending terminal window commands to the Operative
 *          System in which that Class is being run at, and to also receive
 *          responses from sending those commands.
 * 
 * @author  Cesar Miranda Meza (cmirandameza3@hotmail.com)
 * @date    November 01, 2023.
 */
public class TerminalProcess {
    /**@brief   Flag to determine whether the Operative System at which this
     *          class is being run at is Windows with a \c true , or otherwise
     *          with a \c false to indicate that its another Operative System.
     */
    private static boolean isWindows = System.getProperty("os.name").toLowerCase().startsWith("windows");
    
    /**@brief   {@link java.io.File} Object loaded with the root directory path
     *          of the project into which this
     *          {@link com.mortrack.hostbleapp.utils.TerminalProcess} Class is
     *          being used at.
     */
    private static final File PROJECT_LOCATION = new File(System.getProperty("user.dir"));
    
    /**@brief  Executes a desired Terminal Command.
     * 
     * @details Aside from executing a desired Terminal Command, this method
     *          also prints on the Stream some informational notifications (such
     *          as the Directory Path Location at which this Terminal Command
     *          was executed and the actual Terminal Command requested to be
     *          sent) and the received response from sending that Terminal
     *          Command.
     * 
     * @param command   The Terminal Command that wants to be executed.
     * 
     * @note    The implementer is responsible for giving the right value to the
     *          \p command param according to the Operative System from which
     *          the {@link com.mortrack.hostbleapp.utils.TerminalProcess} Class
     *          is being used on.
     * 
     * @param timeout   Timeout in seconds that the implementer wishes this to
     *                  run at the most.
     * @param verbose   Whenever set to \c true , it will show on the Stream
     *                  some informational notifications about the Terminal
     *                  Command requested to be executed and of the response
     *                  received after running it. Otherwise, if its set to
     *                  \c false , then it will not show anything on the Stream.
     * 
     * @note    By Stream, it is meant that {@link java.lang.System.out.println}
     *          will be used to send messages.
     *
     * @throws java.lang.Exception
     * 
     * @author  <a href=https://gist.github.com/Suranchiyev>Beknazar Suranchiyev</a>
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 01, 2023.
     */
    public void runCommand(String command, int timeout, boolean verbose) throws Exception {
        if (verbose) {
            System.out.println("Running in: " + PROJECT_LOCATION);
            System.out.println("Command: " + command);
        }

        ProcessBuilder builder = new ProcessBuilder();
        builder.directory(PROJECT_LOCATION);

        if(isWindows) {
            builder.command("cmd.exe", "/c", command);
        }else {
            builder.command("sh", "-c", command);
        }

        Process process = builder.start();

        OutputStream outputStream = process.getOutputStream();
        InputStream inputStream = process.getInputStream();
        InputStream errorStream = process.getErrorStream();

        if (verbose) {
            printStream(inputStream);
            printStream(errorStream);
        }

        boolean isFinished = process.waitFor(timeout, TimeUnit.SECONDS);
        outputStream.flush();
        outputStream.close();

        if(!isFinished) {
            process.destroyForcibly();
        }
    }
    
    /**@brief  Runs a desired Executable file with respect to the root location
     *          of this project and gets the first response line given after
     *          having executed that file.
     * 
     * @param command   The Execute Terminal Command of the File that wants to
     *                  be executed.
     * 
     * @note    The implementer is responsible for giving the right value to the
     *          \p command param according to the Operative System from which
     *          the {@link com.mortrack.hostbleapp.utils.TerminalProcess} Class
     *          is being used on.
     * @note    An example valid value for the \p command param in Windows is
     *          @code command = ".\\APIs\\main.o 1 2 3" @endcode and for Linux
     *          based O.S. is @code command = "./APIs/main.o 1 2 3" @endcode .
     *
     * @param timeout   Timeout in seconds that the implementer wishes this to
     *                  run at the most.
     * 
     * @return  Provides the first response line given after having executed the
     *          file indicated via the \p command param. However, if there in
     *          the case that there is not response at all, then the \c ""
     *          String value will be given instead.
     * 
     * @throws java.lang.Exception 
     * 
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @author  <a href=https://gist.github.com/Suranchiyev>Beknazar Suranchiyev</a>
     * @date    November 01, 2023.
     */
    public String runExecutableFile(String command, int timeout) throws Exception {
        ProcessBuilder builder = new ProcessBuilder();
        builder.directory(PROJECT_LOCATION);

        if(isWindows) {
            builder.command("cmd.exe", "/c", command);
        }else {
            builder.command("sh", "-c", command);
        }

        Process process = builder.start();

        OutputStream outputStream = process.getOutputStream();
        InputStream inputStream = process.getInputStream();
        InputStream errorStream = process.getErrorStream();

        boolean isFinished = process.waitFor(timeout, TimeUnit.SECONDS);
        outputStream.flush();
        outputStream.close();

        if(!isFinished) {
            process.destroyForcibly();
        }
        
        return getSingleLineFromStream(inputStream);
    }

    /**@brief   Gets only the first line of text from the given Stream, if there
     *          is any text at all.
     * 
     * @param inputStream   Stream from which it is desired to get the first
     *                      line of text.
     * 
     * @return  Gets only the first lien of text from the given Stream via the
     *          \p inputStream param in case that there is any text at all. In
     *          the case that there is no text, then this method will give back
     *          the \c "" String value.
     * 
     * @author  <a href=https://gist.github.com/Suranchiyev>Beknazar Suranchiyev</a>
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 01, 2023.
     */
    private String getSingleLineFromStream(InputStream inputStream) throws IOException {
        String line = "";
        try(BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream))) {
            while((line = bufferedReader.readLine()) != null) {
                break;
            }
        }
        return line;
    }
    
    /**@brief   Gets any available text from the given Stream and then shows it 
     *          via {@link java.lang.System.out.println} .
     * 
     * @param inputStream   Stream from which it is desired to get any available
     *                      text to then show it via
     *                      {@link java.lang.System.out.println} .
     * 
     * @author  <a href=https://gist.github.com/Suranchiyev>Beknazar Suranchiyev</a>
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 01, 2023.
     */
    private void printStream(InputStream inputStream) throws IOException {
        try(BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream))) {
            String line = "";
            while((line = bufferedReader.readLine()) != null) {
                System.out.println(line);
            }
        }
    }
    
    /**@brief   Indicates whether the
     *          {@link com.mortrack.hostbleapp.utils.TerminalProcess} Class is
     *          being used on a Windows O.S. machine or not.
     * 
     * @return \c true if the
     *          {@link com.mortrack.hostbleapp.utils.TerminalProcess} Class is
     *          being used on a Windows O.S. machine. Otherwise, a \c false .
     * 
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 01, 2023.
     */
    public boolean isWindows() {
        return isWindows;
    }
    
    /**@brief   Gets the Absolute Path of the Root Directory Location of the
     *          project on which the
     *          {@link com.mortrack.hostbleapp.utils.TerminalProcess} Class is
     *          being used at.
     * 
     * @return The Absolute Path of the Root Directory location of the project
     *          on which the
     *          {@link com.mortrack.hostbleapp.utils.TerminalProcess} Class is
     *          being used at.
     * 
     * @author  César Miranda Meza (cmirandameza3@hotmail.com)
     * @date    November 01, 2023.
     */
    public String projectLocation() {
        return PROJECT_LOCATION.getAbsolutePath();
    }
}

/** @} */
