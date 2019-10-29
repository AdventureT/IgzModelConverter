#pragma once
#include "MathHelper.h"
#include "IGZ.h"

namespace IgzModelConverterGUI 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace System::IO;
	using namespace msclr::interop;

	//Some unused Skeleton stuff from the hka's
	struct BoneInfo
	{
		FbxNode* m_pNode;
		//FbxSkeleton* m_pSkeleton;
		FbxCluster* m_pCluster;
	};
	struct EulerAngles
	{
		double roll, pitch, yaw;
	};
	struct Quaternion
	{
		double w, x, y, z;
	};

	EulerAngles ToEulerAngles(Quaternion q)
	{
		EulerAngles angles;

		// roll (x-axis rotation)
		double sinr_cosp = +2.0 * (q.w * q.x + q.y * q.z);
		double cosr_cosp = +1.0 - 2.0 * (q.x * q.x + q.y * q.y);
		angles.roll = atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		double sinp = +2.0 * (q.w * q.y - q.z * q.x);
		if (fabs(sinp) >= 1)
			angles.pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
		else
			angles.pitch = asin(sinp);

		// yaw (z-axis rotation)
		double siny_cosp = +2.0 * (q.w * q.z + q.x * q.y);
		double cosy_cosp = +1.0 - 2.0 * (q.y * q.y + q.z * q.z);
		angles.yaw = atan2(siny_cosp, cosy_cosp);

		return angles;
	}
	hkaSkeleton* m_skeleton;
	std::vector<FbxMatrix> matrices;


	// Utility to make sure we always return the right index for the given node
	// Very handy for skeleton hierachy work in the FBX SDK
	FbxNode* GetNodeIndexByName(FbxScene* pScene, std::string NodeName)
	{
		// temp hacky
		FbxNode* NodeToReturn = FbxNode::Create(pScene, "empty");

		for (int i = 0; i < pScene->GetNodeCount(); i++)
		{
			std::string CurrentNodeName = pScene->GetNode(i)->GetName();

			if (CurrentNodeName == NodeName)
			{
				//std::cout << "FOUND BONE AND ITS VALID!!!!!" << "\n";
				NodeToReturn = pScene->GetNode(i);
			}
		}

		return NodeToReturn;
	}

	int GetNodeIDByName(FbxScene* pScene, std::string NodeName)
	{
		int NodeNumber = 0;

		for (int i = 0; i < pScene->GetNodeCount(); i++)
		{
			std::string CurrentNodeName = pScene->GetNode(i)->GetName();

			if (CurrentNodeName == NodeName)
			{
				NodeNumber = i;
			}
		}

		return NodeNumber;
	}
	FbxNode* CreateSkeleton(FbxScene* pScene, const char* pName)
	{
		// get number of bones and apply reference pose
		const int numBones = m_skeleton->GetNumBones();

		// create base limb objects first
		for (int b = 0; b < numBones; b++)
		{
			hkFullBone bone = m_skeleton->GetFullBone(b);
			const hkQTransform* localTransform = m_skeleton->GetBoneTM(b);
			Vector4 pos = localTransform->position;
			Vector4 rot = localTransform->rotation;

			FbxSkeleton* lSkeletonLimbNodeAttribute1 = FbxSkeleton::Create(pScene, pName);

			if ((b == 0))
				lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eRoot);
			else
				lSkeletonLimbNodeAttribute1->SetSkeletonType(FbxSkeleton::eLimbNode);

			lSkeletonLimbNodeAttribute1->Size.Set(1.0);
			FbxNode* BaseJoint = FbxNode::Create(pScene, bone.name);
			BaseJoint->SetNodeAttribute(lSkeletonLimbNodeAttribute1);

			// Set Translation
			BaseJoint->LclTranslation.Set(FbxVector4(pos.X, pos.Y, pos.X));
			//BaseJoint->LclTranslation.Set(matrices[b].GetRow(3));

			// convert quat to euler
			FbxQuaternion test (rot.X, rot.Y, rot.Z, rot.W);
			test.Conjugate();
			Quaternion QuatTest = { test.GetAt(3), test.GetAt(0), test.GetAt(1), test.GetAt(2) };
			EulerAngles inAngs = ToEulerAngles(QuatTest);
			//FbxVector4 angs(rad2deg(inAngs.x), rad2deg(inAngs.y), rad2deg(inAngs.z));
			//angs.FixIncorrectValue();
			
			//FbxQuaternion test(rad2deg(inAngs.x), rad2deg(inAngs.y), rad2deg(inAngs.z), rad2deg(inAngs.w));
			//test.Normalize();
			BaseJoint->LclRotation.Set(FbxVector4(rad2deg(inAngs.pitch), rad2deg(inAngs.roll), rad2deg(inAngs.yaw)));
			//BaseJoint->LclRotation.Set(matrices[b].GetRow(1));
			BaseJoint->LclScaling.Set(FbxVector4(11, 11, 11));
			pScene->GetRootNode()->AddChild(BaseJoint);
		}
		// process parenting and transform now
		for (int c = 0; c < numBones; c++)
		{
			int parent = m_skeleton->GetBoneParentID(c);

			if (parent != -1)
			{
				const char* ParentBoneName = m_skeleton->GetFullBone(parent).name;
				const char* CurrentBoneName = m_skeleton->GetFullBone(c).name;
				std::string CurBoneNameString = CurrentBoneName;
				std::string ParentBoneNameString = ParentBoneName;

				FbxNode* ParentJointNode = GetNodeIndexByName(pScene, ParentBoneName);
				FbxNode* CurrentJointNode = GetNodeIndexByName(pScene, CurrentBoneName);
				ParentJointNode->AddChild(CurrentJointNode);
			}
		}

		return pScene->GetRootNode();
	}
	//Wanted to make a file which holds the paths of each game. This would be more easier than selecting each time you start the program the game paths manual.
	//Might add this to the todo list...
	void WritePathFile(char* nstGamePath, char* ctrGamePath, char* skyGamePath)
	{
		FILE* f;
		fopen_s(&f, "GamePaths.dat", "w");
		fprintf_s(f, "%s %s %s", nstGamePath, ctrGamePath, skyGamePath);
	}

	bool ReadPathFile(char* nstGamePath, char* ctrGamePath, char* skyGamePath)
	{
		FILE* f;
		if (fopen_s(&f, "GamePaths.dat", "rb") != 0)
		{
			return false;
		}
		fgets(nstGamePath, sizeof(nstGamePath), f);
		return true;
	}

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{

			int _game = 0;

			 int _type = 0;
			 bool _fileChosen = false;
	private: System::Windows::Forms::RadioButton^ skyl;


	private: System::Windows::Forms::ProgressBar^ progressBar1;
			 char* _filename = new char[256];
			 char* _filenamehkx = new char[256];

			 char* _path = new char[256];
	private: System::Windows::Forms::Button^ openFolder;


	private: System::Windows::Forms::OpenFileDialog^ openFileDialog2;
	private: System::Windows::Forms::TreeView^ treeView1;
	private: System::Windows::Forms::Button^ gameFolder;

	private: System::Windows::Forms::FolderBrowserDialog^ folderBrowserDialog1;
			bool busy = false;
			 int _files = 0;
	private: System::Windows::Forms::Button^ convert;
		   //char* nstGamePath;
			 //char* ctrGamePath;


		   //char* skyGamePath;
			 bool _bones = false;

	public:
		MyForm(void)
		{
			InitializeComponent();
			/*
			if (ReadPathFile(nstGamePath, ctrGamePath, skyGamePath) == false)
			{
				
				//Create a new Path file
				WritePathFile(nstGamePath, ctrGamePath, skyGamePath);
			}
			*/
			//
			//TODO: Add the constructor code here
			//
		}
	public:
		bool ExecuteConvertion(void)
		{
			try
			{
				IGZ::IGZ igZ(_game, _type, _filename);
			}
			catch (System::Exception^ ex)
			{
				MessageBox::Show(ex->Message, "Error",
					MessageBoxButtons::OK, MessageBoxIcon::Error);
				return false;
			}
			progressBar1->Increment(100 / _files);
			return true;
		}
	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::OpenFileDialog^  openFileDialog1;
	private: System::Windows::Forms::RadioButton^  NSTButton;
	private: System::Windows::Forms::RadioButton^  CTRButton;
	private: System::Windows::Forms::Panel^  panel1;
	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MyForm::typeid));
			this->openFileDialog1 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->NSTButton = (gcnew System::Windows::Forms::RadioButton());
			this->CTRButton = (gcnew System::Windows::Forms::RadioButton());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->skyl = (gcnew System::Windows::Forms::RadioButton());
			this->progressBar1 = (gcnew System::Windows::Forms::ProgressBar());
			this->openFolder = (gcnew System::Windows::Forms::Button());
			this->openFileDialog2 = (gcnew System::Windows::Forms::OpenFileDialog());
			this->treeView1 = (gcnew System::Windows::Forms::TreeView());
			this->gameFolder = (gcnew System::Windows::Forms::Button());
			this->folderBrowserDialog1 = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->convert = (gcnew System::Windows::Forms::Button());
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// openFileDialog1
			// 
			this->openFileDialog1->FileName = L"openFileDialog1";
			// 
			// NSTButton
			// 
			this->NSTButton->AutoSize = true;
			this->NSTButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->NSTButton->Location = System::Drawing::Point(3, 3);
			this->NSTButton->Name = L"NSTButton";
			this->NSTButton->Size = System::Drawing::Size(50, 17);
			this->NSTButton->TabIndex = 3;
			this->NSTButton->TabStop = true;
			this->NSTButton->Text = L"NST";
			this->NSTButton->UseVisualStyleBackColor = true;
			this->NSTButton->CheckedChanged += gcnew System::EventHandler(this, &MyForm::NSTButton_CheckedChanged);
			// 
			// CTRButton
			// 
			this->CTRButton->AutoSize = true;
			this->CTRButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->CTRButton->Location = System::Drawing::Point(56, 3);
			this->CTRButton->Name = L"CTRButton";
			this->CTRButton->Size = System::Drawing::Size(50, 17);
			this->CTRButton->TabIndex = 4;
			this->CTRButton->TabStop = true;
			this->CTRButton->Text = L"CTR";
			this->CTRButton->UseVisualStyleBackColor = true;
			this->CTRButton->CheckedChanged += gcnew System::EventHandler(this, &MyForm::CTRButton_CheckedChanged);
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::Transparent;
			this->panel1->Controls->Add(this->skyl);
			this->panel1->Controls->Add(this->NSTButton);
			this->panel1->Controls->Add(this->CTRButton);
			this->panel1->Location = System::Drawing::Point(15, 27);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(205, 26);
			this->panel1->TabIndex = 5;
			// 
			// skyl
			// 
			this->skyl->AutoSize = true;
			this->skyl->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->skyl->Location = System::Drawing::Point(112, 3);
			this->skyl->Name = L"skyl";
			this->skyl->Size = System::Drawing::Size(91, 17);
			this->skyl->TabIndex = 5;
			this->skyl->TabStop = true;
			this->skyl->Text = L"SkyLanders";
			this->skyl->UseVisualStyleBackColor = true;
			this->skyl->CheckedChanged += gcnew System::EventHandler(this, &MyForm::skylRadioButton_CheckedChanged);
			// 
			// progressBar1
			// 
			this->progressBar1->Location = System::Drawing::Point(12, 456);
			this->progressBar1->Name = L"progressBar1";
			this->progressBar1->Size = System::Drawing::Size(799, 26);
			this->progressBar1->TabIndex = 7;
			// 
			// openFolder
			// 
			this->openFolder->Location = System::Drawing::Point(335, 221);
			this->openFolder->Name = L"openFolder";
			this->openFolder->Size = System::Drawing::Size(113, 23);
			this->openFolder->TabIndex = 8;
			this->openFolder->Text = L"Open in Explorer";
			this->openFolder->UseVisualStyleBackColor = true;
			this->openFolder->Click += gcnew System::EventHandler(this, &MyForm::openFolderButton_Click);
			// 
			// openFileDialog2
			// 
			this->openFileDialog2->FileName = L"openFileDialog2";
			// 
			// treeView1
			// 
			this->treeView1->Location = System::Drawing::Point(15, 76);
			this->treeView1->Name = L"treeView1";
			this->treeView1->Size = System::Drawing::Size(282, 374);
			this->treeView1->TabIndex = 10;
			// 
			// gameFolder
			// 
			this->gameFolder->Location = System::Drawing::Point(303, 163);
			this->gameFolder->Name = L"gameFolder";
			this->gameFolder->Size = System::Drawing::Size(172, 23);
			this->gameFolder->TabIndex = 11;
			this->gameFolder->Text = L"Choose your game folder";
			this->gameFolder->UseVisualStyleBackColor = true;
			this->gameFolder->Click += gcnew System::EventHandler(this, &MyForm::gameFolderButton_Click);
			// 
			// convert
			// 
			this->convert->Location = System::Drawing::Point(352, 192);
			this->convert->Name = L"convert";
			this->convert->Size = System::Drawing::Size(75, 23);
			this->convert->TabIndex = 13;
			this->convert->Text = L"Convert";
			this->convert->UseVisualStyleBackColor = true;
			this->convert->Click += gcnew System::EventHandler(this, &MyForm::convertButton_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackgroundImage = (cli::safe_cast<System::Drawing::Image^>(resources->GetObject(L"$this.BackgroundImage")));
			this->ClientSize = System::Drawing::Size(823, 494);
			this->Controls->Add(this->convert);
			this->Controls->Add(this->gameFolder);
			this->Controls->Add(this->treeView1);
			this->Controls->Add(this->openFolder);
			this->Controls->Add(this->progressBar1);
			this->Controls->Add(this->panel1);
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->Name = L"MyForm";
			this->Text = L"IgzModelConverterGUI v1.3";
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void NSTButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		_game = 1;
	}
	private: System::Void CTRButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		_game = 2;
	}
	private: System::Void skylRadioButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		_game = 3;
	}
	private: System::Void ActorButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		_type = 1;
	}
	private: System::Void ModelButton_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
	{
		_type = 2;
	}

	private: System::Void openFolderButton_Click(System::Object^ sender, System::EventArgs^ e) {
		if (!_fileChosen)
		{
			MessageBox::Show("Please choose your game path folder first!", "Error",
				MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}
		if (_filename[0] == -51)
		{
			MessageBox::Show("Please choose your Igz Model file first (you need to convert it)!", "Error",
				MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}
		std::string filename = _filename;
		std::string directory;
		const size_t last_slash_idx = filename.rfind('\\');
		if (std::string::npos != last_slash_idx)
		{
			directory = filename.substr(0, last_slash_idx);
		}
		ShellExecute(NULL, "open", directory.c_str(), NULL, NULL, SW_SHOWMINIMIZED);
}

	private: System::Void gameFolderButton_Click(System::Object^ sender, System::EventArgs^ e)
	{
		if (folderBrowserDialog1->ShowDialog() == Windows::Forms::DialogResult::OK)
		{
			_fileChosen = true;
			_path = (char*)(void*)Marshal::StringToHGlobalAnsi(folderBrowserDialog1->SelectedPath);
		}
		System::String^ sFileName = gcnew String(_path);
		treeView1->CheckBoxes = true;
		DirectoryInfo^ directoryInfo = gcnew DirectoryInfo(sFileName);
		BuildTree(directoryInfo, treeView1->Nodes);
	}

	private: System::Void convertButton_Click(System::Object^ sender, System::EventArgs^ e) {
		progressBar1->Value = 0;
		System::Collections::IEnumerator^ countEnum = treeView1->Nodes->GetEnumerator();
		while (countEnum->MoveNext())
		{
			GetCountOfCheckedNodes(safe_cast<TreeNode^>(countEnum->Current));
		}
		System::Collections::IEnumerator^ checkedEnum = treeView1->Nodes->GetEnumerator();
		while (checkedEnum->MoveNext())
		{
			GetCheckedNodes(safe_cast<TreeNode^>(checkedEnum->Current));
		}
		MessageBox::Show(String::Format("{0} Files converted!", _files), "Success", MessageBoxButtons::OK, MessageBoxIcon::Information);
		_files = 0;
	}
	private: void GetCheckedNodes(TreeNode^ Node)
	{
		if (Node->Checked)
		{
			std::string fn = msclr::interop::marshal_as<std::string>(Node->Name);
			if (fn.substr(fn.find_last_of(".") + 1) == "igz")
			{
				bool ok = true;
				if (Node->Name->Contains("actors"))
				{
					_filenamehkx = GetAnimPath(Node->Name);
					_type = 1;
				}
				else if (Node->Name->Contains("models"))
				{
					_type = 2;
				}
				else if (Node->Name->Contains("sky"))
				{
					_type = 3;
				}
				else
				{
					throw gcnew System::Exception(gcnew System::String("Unknown Type"));
				}
				_fileChosen = true;
				_filename = (char*)(void*)Marshal::StringToHGlobalAnsi(Node->Name);
				ok = ExecuteConvertion();
				if (!ok)
				{
					throw gcnew System::Exception(gcnew System::String("Some Error occuried..."));
				}
			}
		}

		for each (TreeNode^ subNode in Node->Nodes)
		{
			GetCheckedNodes(subNode);
		}
	}
	private: void GetCountOfCheckedNodes(TreeNode^ Node)
	{
		if (Node->Checked)
		{
			std::string fn = msclr::interop::marshal_as<std::string>(Node->Name);
			if (fn != "") //fn is "" if it is a directory
			{
				_files++;
			}
		}

		for each (TreeNode^ subNode in Node->Nodes)
		{
			GetCountOfCheckedNodes(subNode);
		}
	}
	private: void BuildTree(DirectoryInfo^ directoryInfo, TreeNodeCollection^ addInMe)
	{
		TreeNode^ curNode = addInMe->Add(directoryInfo->Name);
		cli::array< DirectoryInfo^ >^ subDir = directoryInfo->GetDirectories();
		for (int i = 0; i < subDir->Length; i++)
		{
			BuildTree(subDir[i], curNode->Nodes);
		}
		cli::array< FileInfo^ >^ subFiles = directoryInfo->GetFiles();

		for (int i = 0; i < subFiles->Length; i++)
		{
			if (subFiles[i]->Extension == gcnew String(".igz"))
			{
				curNode->Nodes->Add(subFiles[i]->FullName, subFiles[i]->Name);
			}
		}
	}
	private: char* GetAnimPath(System::String^ path)
	{
		std::string curPath = msclr::interop::marshal_as<std::string>(path);
		std::vector<std::string> subdirs;
		std::string base_filename;
		std::string root;
		do
		{
			root = curPath.substr(0, curPath.find_last_of("/\\"));
			base_filename = curPath.substr(curPath.find_last_of("/\\") + 1);
			if (base_filename == "actors")
			{
				break;
			}
			subdirs.push_back(base_filename);
			curPath = curPath.substr(0, curPath.find_last_of("/\\"));
		} while (base_filename != "actors");
		std::string hkxPath = root;
		hkxPath.append("\\anims");
		std::reverse(subdirs.begin(), subdirs.end());
		for (size_t i = 0; i < subdirs.size(); i++)
		{
			hkxPath.append("\\" + subdirs[i]);
		}
		std::string::size_type i = hkxPath.rfind('.', hkxPath.length());
		std::string newExt = "hka";
		if (i != std::string::npos) {
			hkxPath.replace(i + 1, newExt.length(), newExt);
		}
		System::String^ test2 = gcnew String(hkxPath.c_str());
		return (char*)(void*)Marshal::StringToHGlobalAnsi(test2);
	}

};
}
