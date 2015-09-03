using UnityEngine;
using System.Collections;
using System.IO;

/*
	Help to read in external .obj files.
	
*/
public class readObj{

	public readObj(){

	}


	public string write(string name)
	{
		string result = "";
		string path = Application.dataPath;//@"/Users/nuonuomisu/Desktop/lushumin/1.txt";

		string toFind = "";


		if (Application.platform == RuntimePlatform.WindowsPlayer || Application.platform == RuntimePlatform.WindowsEditor)
		{
			toFind = "\\";
		}
		if (Application.platform == RuntimePlatform.OSXPlayer)
		{
			toFind = "/";
		}


		for (int i = 0; i< 2; i++){
			int pos = path.LastIndexOf (toFind);
			path = path.Substring(0, pos);
		}

		if (Application.platform == RuntimePlatform.WindowsPlayer || Application.platform == RuntimePlatform.WindowsEditor)
		{
			path += "\\mesh\\"+name;
		}
		if (Application.platform == RuntimePlatform.OSXPlayer)
		{
			path += "/mesh/"+name;
		}



		if (!File.Exists(path))
		{
		
		}
		// Open the file to read from. 
		using (StreamReader sr = File.OpenText(path))
		{
			string s = "";

			while ((s = sr.ReadLine()) != null)
			{
				result +=s + "\n";
			} 
		}

		return result;
	}
}
