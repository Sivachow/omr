/*******************************************************************************
 * Copyright (c) 2010, 2014 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/
package com.ibm.j9ddr.autoblob;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

import com.ibm.j9ddr.autoblob.config.Configuration;

/**
 * Generates the C file containing the header includes
 * to be pre-processed.
 * 
 * Usage:
 * 
 * java com.ibm.j9ddr.autoblob.GenerateInputCFile <config file> <output file>
 * 
 * @author andhall
 *
 */
public class GenerateInputCFile
{

	private static final int EXPECTED_NUMBER_OF_ARGUMENTS = 2;

	/**
	 * @param args
	 * @throws IOException 
	 */
	public static void main(String[] args) throws IOException
	{
		printHeader();
		
		if (args.length != EXPECTED_NUMBER_OF_ARGUMENTS) {
			System.err.println("Unexpected number of arguments. Expected " + EXPECTED_NUMBER_OF_ARGUMENTS + ", actually got " + args.length);
			usage();
			System.exit(1);
		}

		File configFile = new File(args[0]);
		
		if (! configFile.exists()) {
			System.err.println("Specified config file: " + configFile.getAbsolutePath() + " does not exist");
			System.exit(1);
		}
		
		Configuration config = Configuration.loadConfiguration(configFile, null);
		
		File outputFile = new File(args[1]);
		
		writeCFile(outputFile, config);
	}

	private static void writeCFile(File outputFile, Configuration config) throws IOException
	{
		PrintWriter writer = new PrintWriter(new FileWriter(outputFile));
		
		try {
			System.out.println("Writing C file: " + outputFile.getAbsolutePath());
			writeCFile(writer, config);
			if (writer.checkError()) {
				System.out.println("Error during write");
			} else {
				System.out.println("C file written");
			}
		} finally {
			writer.close();
		}
		
	}

	private static void writeCFile(PrintWriter writer, Configuration config)
	{
		writer.println("/* GENERATED BY J9DDR AUTOBLOB GenerateInputCFile. DO NOT EDIT */");
		writer.println();
		writer.println();
		config.writeCIncludes(writer);
	}

	private static void printHeader()
	{
		System.out.println("J9DDR Autoblob GenerateInputCFile");
	}

	private static void usage()
	{
		System.err.println();
		System.err.println("java com.ibm.j9ddr.autoblob.GenerateInputCFile <config file> <output file>");
	}

}