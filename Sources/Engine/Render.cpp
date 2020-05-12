
#include "Render.hpp"


using namespace Engine;

CRender::CRender(void)
{
}

CRender::~CRender(void)
{
    if(Track) {
        delete Track;
        Track = NULL;
        Logger->Text("Destroyed Track Object");
    }

    if(PodPool) {
        delete PodPool;
        PodPool = NULL;
        Logger->Text("Destroyed Pod Pool Object");
    }

    if(Vehicle) {

        if(Config->Physics.Save) {
            Vehicle->SavePhysics(Config->Physics.VehicleFile);
            Logger->Text("Saved Vehicle Physics");
        }

        Physics::CServer::GetInstance()->UnregisterClient(Vehicle);
        Logger->Text("Unregistered Vehicle Client From Server");

        delete Vehicle;
        Vehicle = NULL;
        Logger->Text("Destroyed Vehicle Object");
    }

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


bool CRender::Create(Game::CPodPool &PodPool, Game::CTrack &Track, Physics::CVehicle &Vehicle, Engine::CTimer &Timer, std::string &InterfaceFile)
{
    this->PodPool = &PodPool;
    this->Track = &Track;
    this->Vehicle = &Vehicle;
    this->Timer = &Timer;
    this->InterfaceFile = InterfaceFile;

    Weather.Create(PodPool, Config->Game.Weather);

    if(QuadTree.Create(&Track, Track.Dimensions() / 8)) {
        Logger->Text("Created Terrain QuadTree");
    } else {
        Logger->Error("Error Creating Terrain QuadTree");
    }

    LogoTexture.LoadImage("Logo.png", "Game\\");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NavEnableSetMousePos | ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = &this->InterfaceFile.c_str()[0];
    ImGui_ImplGlfw_InitForOpenGL(GLWindow, true);
    ImGui_ImplOpenGL2_Init();

    ImGuiStyle& Style = ImGui::GetStyle();

    Style.WindowRounding = 2.0f;
    Style.FrameRounding = 2.0f;
    Style.ScrollbarRounding = 2.0f;
    Style.WindowBorderSize = 2.0f;
    Style.FrameBorderSize = 2.0f;

    Style.Colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    Style.Colors[ImGuiCol_WindowBg]               = ImVec4(0.20f, 0.20f, 0.20f, 0.75f);
    Style.Colors[ImGuiCol_ChildBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
    Style.Colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    Style.Colors[ImGuiCol_Border]                 = ImVec4(0.61f, 0.61f, 0.61f, 0.50f);
    Style.Colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    Style.Colors[ImGuiCol_FrameBg]                = ImVec4(0.45f, 0.45f, 0.45f, 0.54f);
    Style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.29f, 0.30f, 0.31f, 0.40f);
    Style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.49f, 0.46f, 0.46f, 0.67f);
    Style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
    Style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.66f, 0.19f, 0.19f, 1.00f);
    Style.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    Style.Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    Style.Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    Style.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    Style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    Style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    Style.Colors[ImGuiCol_CheckMark]              = ImVec4(0.87f, 0.37f, 0.38f, 1.00f);
    Style.Colors[ImGuiCol_SliderGrab]             = ImVec4(0.54f, 0.54f, 0.54f, 1.00f);
    Style.Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.90f, 0.15f, 0.15f, 1.00f);
    Style.Colors[ImGuiCol_Button]                 = ImVec4(0.45f, 0.45f, 0.45f, 0.40f);
    Style.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.84f, 0.17f, 0.17f, 1.00f);
    Style.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    Style.Colors[ImGuiCol_Header]                 = ImVec4(0.52f, 0.52f, 0.52f, 0.31f);
    Style.Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.80f, 0.11f, 0.11f, 0.80f);
    Style.Colors[ImGuiCol_HeaderActive]           = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
    Style.Colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    Style.Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.39f, 0.39f, 0.39f, 0.78f);
    Style.Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
    Style.Colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.25f);
    Style.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.49f, 0.49f, 0.49f, 0.67f);
    Style.Colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.50f, 0.10f, 0.10f, 0.95f);
    Style.Colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    Style.Colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.98f, 0.26f, 0.26f, 0.48f);
    Style.Colors[ImGuiCol_DragDropTarget]         = ImVec4(0.57f, 0.57f, 0.57f, 0.90f);
    Style.Colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.27f, 0.27f, 1.00f);
    Style.Colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    Style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    Style.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);


    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
//    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
//    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glClearStencil(0);

    glAlphaFunc(GL_GEQUAL, 0.75f);
    glEnable(GL_ALPHA_TEST);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 2.0f);

    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);

    glPolygonMode(GL_FRONT, GL_FILL);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_NORMALIZE);
    glLightfv(GL_LIGHT0, GL_POSITION, &Track.GetSunPosition()[0]);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    Resize();

    Config->General.Paused = false;

    return true;

}

void CRender::Pass(float Delta, bool Invert)
{

    if(!Invert) {

        if(Config->Render.Backdrop) {
            Backdrop(false);
        }

        Weather.ApplyTint();

        for(int x = -Config->Render.Wrap; x <= Config->Render.Wrap; x++) {
            for(int z = -Config->Render.Wrap; z <= Config->Render.Wrap; z++) {

                if(glm::distance(glm::vec3(x * Track->Dimensions() + Track->Dimensions() / 2.0f, 0, z * Track->Dimensions() + Track->Dimensions() / 2.0f), Camera.GetPosition()) <  Track->Dimensions()) {

                    glPushMatrix();

                    glTranslatef((x * Track->Dimensions()), 0.0f, (z * Track->Dimensions()) );
                    Terrain(QuadTree.GetRootNode(), glm::vec3(x, Camera.GetPosition().y, z), false);

                    glPopMatrix();
                }
            }
        }



        Vehicle->Render();

        if(Config->Render.Models) {
            Models(false);
        }

    } else {

        if(Config->Render.Backdrop) {
            Backdrop(true);
        }

        glPushMatrix();

        Weather.ApplyTint();

        glTranslatef(0.0f, Track->GetWaterHeight() * 2, 0.0f);
        glScalef(1.0f, -1.0f, 1.0f);

        glm::vec3 Normal = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));
        float Distance = -glm::dot(Normal, glm::vec3(Track->Dimensions(), Track->GetWaterHeight(), Track->Dimensions()));

        glEnable(GL_CLIP_PLANE0);
        GLdouble PlaneEqn[] = { Normal.x, Normal.y, Normal.z, Distance };
        glClipPlane(GL_CLIP_PLANE0, PlaneEqn);

        Terrain(QuadTree.GetRootNode(), glm::vec3(0, Camera.GetPosition().y, 0), true);

        if(Config->Render.Models) {
            Models(true);
        }

        Vehicle->Render();

        glDisable(GL_CLIP_PLANE0);

        glPopMatrix();

    }

    if(Config->Render.Water) {
        Water(Timer->GetDelta());
    }

}

void CRender::Scene(void)
{

    if(Config->General.EditMode) {

        Config->Render.Wireframe ? glPolygonMode(GL_FRONT, GL_LINE) : glPolygonMode(GL_FRONT, GL_FILL);
        Config->Render.Textured ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);
        Config->Render.Culled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
        Config->Render.Lighted ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);

    } else {

        glPolygonMode(GL_FRONT, GL_FILL);
        glEnable(GL_LIGHTING);
        glEnable(GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
    }

    glCallList(PerspectiveView);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4x4 LookAt = Camera.Update(Timer->GetDelta(), Vehicle);
    glMultMatrixf(glm::value_ptr(LookAt));
    Frustum.UpdateFast();

    glLightfv(GL_LIGHT0, GL_POSITION, &Track->GetSunPosition()[0]);
    LightPosition = glm::vec4(Track->GetSunPosition().y, 256.0f, Track->GetSunPosition().z, 1.0f);

    if(Config->Render.Reflections && Config->Render.Water && Camera.GetPosition().y > Track->GetWaterHeight()) {
        Reflections();
    }

    Pass(Timer->GetDelta(), false);

    if(Config->Render.Water) {
        Water(Timer->GetDelta());
    }

    float Matrix[16];
    glm::vec4 Lp;
    glm::vec4 Wp;

    if (Config->Render.Shadows) {

        glClear(GL_STENCIL_BUFFER_BIT);

        for (int i = 0; i < Track->NumModels(); i++) {
            if (Objects[i].Type == dTriMeshClass) {

                glPushMatrix();
                glMultMatrixd(Objects[i].GetMatrix());

//                glm::mat4x4 Inverse = glm::inverse(glm::mat4x4(Objects[i].GetMatrix()[0],
//                                                  Objects[i].GetMatrix()[1],
//                                                  Objects[i].GetMatrix()[2],
//                                                  Objects[i].GetMatrix()[3],
//                                                  Objects[i].GetMatrix()[4],
//                                                  Objects[i].GetMatrix()[5],
//                                                  Objects[i].GetMatrix()[6],
//                                                  Objects[i].GetMatrix()[7],
//                                                  Objects[i].GetMatrix()[8],
//                                                  Objects[i].GetMatrix()[9],
//                                                  Objects[i].GetMatrix()[10],
//                                                  Objects[i].GetMatrix()[11],
//                                                  Objects[i].GetMatrix()[12],
//                                                  Objects[i].GetMatrix()[13],
//                                                  Objects[i].GetMatrix()[14],
//                                                  Objects[i].GetMatrix()[15]));
//                glMultMatrixf(glm::value_ptr(Inverse));
//
//                glGetFloatv(GL_MODELVIEW_MATRIX, Matrix);
//
//                Lp = LightPosition;
//                glm::mat4x4 MatrixLocal = glm::mat4x4(Matrix[0],
//                                                  Matrix[1],
//                                                  Matrix[2],
//                                                  Matrix[3],
//                                                  Matrix[4],
//                                                  Matrix[5],
//                                                  Matrix[6],
//                                                  Matrix[7],
//                                                  Matrix[8],
//                                                  Matrix[9],
//                                                  Matrix[10],
//                                                  Matrix[11],
//                                                  Matrix[12],
//                                                  Matrix[13],
//                                                  Matrix[14],
//                                                  Matrix[15]);
//
//                Lp = MatrixLocal * Lp;
//
//                glMultMatrixd(Objects[i].GetMatrix());
//
//                glGetFloatv(GL_MODELVIEW_MATRIX, Matrix);
//
//                glm::mat4x4 MatrixWorld = glm::mat4x4(Matrix[0],
//                                                  Matrix[1],
//                                                  Matrix[2],
//                                                  Matrix[3],
//                                                  Matrix[4],
//                                                  Matrix[5],
//                                                  Matrix[6],
//                                                  Matrix[7],
//                                                  Matrix[8],
//                                                  Matrix[9],
//                                                  Matrix[10],
//                                                  Matrix[11],
//                                                  Matrix[12],
//                                                  Matrix[13],
//                                                  Matrix[14],
//                                                  Matrix[15]);
//
//                Wp = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
//                Wp = MatrixWorld * Wp;
//                Lp = Wp + Lp;
//
//                LightPosition = Lp;

                Shadows(i, LightPosition);
                glPopMatrix();
            }
        }
    }


    Interface();

    glfwSwapBuffers(GLWindow);

}

void CRender::Shadows(int Index, glm::vec4 LightPosition)
{
	unsigned int k;
	unsigned int jj;
	unsigned int p1, p2;
	glm::vec3 v1, v2;
	float Side;

//	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
//    glCullFace(GL_FRONT);

	Game::CModel &Model = Track->GetModel(Index);

//    Model.CalculatePlanes(Objects[Index].GetPosition());

	for (unsigned int i = 0; i < Model.Faces.size(); i++){
		Side = Model.Faces[i].PlaneEq.x * LightPosition[0] +
               Model.Faces[i].PlaneEq.y * LightPosition[1] +
               Model.Faces[i].PlaneEq.z * LightPosition[2] +
               Model.Faces[i].PlaneEq.w * LightPosition[3];
		if (Side > 0) {
            Model.Faces[i].Visible = true;
		} else {
            Model.Faces[i].Visible = false;
		}
	}

 	glDisable(GL_LIGHTING);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_STENCIL_TEST);
	glColorMask(0, 0, 0, 0);
	glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

	glFrontFace(GL_CCW);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

//	glPushMatrix();

    for(unsigned int i = 0; i < Model.Faces.size(); i++){
        if (Model.Faces[i].Visible) {
			for(int j = 0; j < 3; j++){
//				k = Model.Faces[i].Neighbors[j];
//				if ((!k) || (!Model.Faces[k-1].Visible)){

					p1 = Model.Faces[i].Vertices[j].Index;
					jj = (j + 1) % 3;
					p2 = Model.Faces[i].Vertices[jj].Index;

                    v1.x = (Model.Vertices[p1].x) - LightPosition[0];
                    v1.y = (Model.Vertices[p1].y) - LightPosition[1];
                    v1.z = (Model.Vertices[p1].z) - LightPosition[2];

                    v2.x = (Model.Vertices[p2].x) - LightPosition[0];
                    v2.y = (Model.Vertices[p2].y) - LightPosition[1];
                    v2.z = (Model.Vertices[p2].z) - LightPosition[2];

					glBegin(GL_TRIANGLE_STRIP);
						glVertex3f(Model.Vertices[p1].x, Model.Vertices[p1].y, Model.Vertices[p1].z);
						glVertex3f(Model.Vertices[p1].x + v1.x, Model.Vertices[p1].y + v1.y, Model.Vertices[p1].z + v1.z);
						glVertex3f(Model.Vertices[p2].x, Model.Vertices[p2].y, Model.Vertices[p2].z);
						glVertex3f(Model.Vertices[p2].x + v2.x, Model.Vertices[p2].y + v2.y, Model.Vertices[p2].z + v2.z);
					glEnd();

//				}
			}
        }
	}

	glFrontFace(GL_CW);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);

    for(unsigned int i = 0; i < Model.Faces.size(); i++){
        if (Model.Faces[i].Visible) {
			for(int j = 0; j < 3; j++){
//				k = Model.Faces[i].Neighbors[j];
//				if ((!k) || (!Model.Faces[k-1].Visible)){

					p1 = Model.Faces[i].Vertices[j].Index;
					jj = (j + 1) % 3;
					p2 = Model.Faces[i].Vertices[jj].Index;

                    v1.x = (Model.Vertices[p1].x) - LightPosition[0];
                    v1.y = (Model.Vertices[p1].y) - LightPosition[1];
                    v1.z = (Model.Vertices[p1].z) - LightPosition[2];

                    v2.x = (Model.Vertices[p2].x) - LightPosition[0];
                    v2.y = (Model.Vertices[p2].y) - LightPosition[1];
                    v2.z = (Model.Vertices[p2].z) - LightPosition[2];

					glBegin(GL_TRIANGLE_STRIP);
						glVertex3f(Model.Vertices[p1].x, Model.Vertices[p1].y, Model.Vertices[p1].z);
						glVertex3f(Model.Vertices[p1].x + v1.x, Model.Vertices[p1].y + v1.y, Model.Vertices[p1].z + v1.z);
						glVertex3f(Model.Vertices[p2].x, Model.Vertices[p2].y, Model.Vertices[p2].z);
						glVertex3f(Model.Vertices[p2].x + v2.x, Model.Vertices[p2].y + v2.y, Model.Vertices[p2].z + v2.z);
					glEnd();
//				}
			}
        }
	}

//	glPopMatrix();

	glFrontFace(GL_CCW);
	glColorMask(1, 1, 1, 1);

	glColor4f(0.0f, 0.0f, 0.0f, 0.045f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glStencilFunc(GL_NOTEQUAL, 0, 0xffffffff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_TRIANGLE_STRIP);
		glVertex3f(-0.1f, 0.1f,-0.10f);
		glVertex3f(-0.1f,-0.1f,-0.10f);
		glVertex3f( 0.1f, 0.1f,-0.10f);
		glVertex3f( 0.1f,-0.1f,-0.10f);
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);

	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_LIGHTING);
	glDisable(GL_STENCIL_TEST);
	glShadeModel(GL_SMOOTH);



}

void CRender::Reflections(void)
{
    glClear(GL_STENCIL_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

    Water(Timer->GetDelta());

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);

    glStencilFunc(GL_EQUAL, 1, 0xffffffff);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    Pass(Timer->GetDelta(), true);

    glDisable(GL_STENCIL_TEST);

}

void CRender::Editor(void)
{

}

void CRender::Terrain(CNode *Node, glm::vec3 Offset, bool Invert)
{

    if(Node->Leaf) {

        if (Frustum.SphereInFrustum(glm::vec3(Node->Center.x + Offset.x * Track->Dimensions(), Offset.y, Node->Center.z + Offset.z * Track->Dimensions()), QuadTree.GetLeafSize()) > CFrustum::Outside) {
//            if (glm::distance(glm::vec3(Node->Center.x + Offset.x * Track->Dimensions(), Node->Center.y, Node->Center.z + Offset.z * Track->Dimensions()), Camera.GetPosition()) <  Config->Graphics.FarClip) {
                Node->Render(Invert);
//            }
        }

    } else {
        for(int i = 0; i < 4; i++) {
            Terrain(Node->Child[i], Offset, Invert);
        }
    }
}

void CRender::Interface(void)
{

    glAlphaFunc(GL_GEQUAL, 0.75f);

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    if(Config->General.Paused) {

        Logo();

        ImGui::Begin("Project Monster", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoTitleBar*/);

        ImGui::SetWindowSize(ImVec2(300, 320), 0);
        ImGui::SetWindowPos(ImVec2(float ((Width / 2) - 150), (float (Height / 2) - Height / 8)));

        ImGui::PushItemWidth(230);

        if(ImGui::CollapsingHeader("General")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Help"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Physics"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Game"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Render"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 0);

            if(ImGui::Button("Load Defaults... ")) {

                if(MessageBox(NULL, "Defaults Will Be Loaded And Restart Required, Would You Like To Continue?", "Defaults", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                    Config->Defaults();
                    Loop = false;
                }
            }

            ImGui::SameLine();

            if(ImGui::Button("Restart Monster...")) {
                Loop = false;
            }


            ImGui::Checkbox("Edit Mode", &Config->General.EditMode);
            ImGui::SameLine();
            ImGui::Checkbox("Physics Mode", &Config->General.PhysicsMode);

            ImGui::Checkbox("Debug Mode", &Config->General.DebugMode);
            ImGui::SameLine();
            ImGui::Checkbox("Fps Counter", &Config->General.FpsCounter);

        }

        if(ImGui::CollapsingHeader("Game")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Game"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Help"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Physics"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Game"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Render"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 0);

            ImGui::Checkbox("Use Pod Files", &Config->Game.Pods);
            ImGui::SameLine();
            ImGui::Checkbox("Use Loose Files", &Config->Game.Files);

            ImGui::Text("Pod INI File - %s", Config->Game.PodIni);
            ImGui::SameLine();

            if(ImGui::Button("...")) {
                std::string File = FileOpen(NULL, (char*)("Pod INI Files\0*.ini\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir)[0], &std::string(".ini")[0]);

                if(File.size() != 0) {
                    strcpy(Config->Game.PodIni, File.substr(File.find_last_of("\\") + 1).c_str());

                    if(MessageBox(NULL, "Changes Require A Restart, Would You Like To Continue?", "Restart", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        Loop = false;
                    }
                }
            }

            char UMin, UMax = 8;

            static int Index = 0;
            ImGui::SliderScalar("Weather", ImGuiDataType_U8, &Config->Game.Weather, &UMin, &UMax, "%i");

            if(Config->Game.Weather != Index) {
                Weather.Create(*PodPool, Config->Game.Weather);
                Index = Config->Game.Weather;
            }

            UMin = 0;
            UMax = 3;
            ImGui::SliderScalar("Scenery", ImGuiDataType_U8, &Config->Game.Scenery, &UMin, &UMax, "%i");

            static int LastTruck = 0;
            static int ComboBox_TruckItem = 0;

            for(unsigned int i = 0; i < PodPool->NumTrucks(); i++) {
                if(StrCompare(&PodPool->TruckFilename(i), Config->Game.Vehicle)) {
                    ComboBox_TruckItem = i;
                    LastTruck = ComboBox_TruckItem;
                    continue;
                }
            }

            static char **TruckItems = new char*[PodPool->NumTrucks()];

            for(unsigned int i = 0; i < PodPool->NumTrucks(); i++) {
//                TruckItems[i] = &PodPool->TruckFilename(i);
                TruckItems[i] = &PodPool->TruckName(i);
            }


            ImGui::Combo("Truck", &ComboBox_TruckItem, TruckItems, PodPool->NumTrucks(), 10);


            if(ComboBox_TruckItem != LastTruck) {

                LastTruck = ComboBox_TruckItem;

                strcpy(Config->Game.Vehicle, &PodPool->TruckFilename(ComboBox_TruckItem));

//                Loop = false;

                if(Vehicle) {

                    Physics::CServer::GetInstance()->UnregisterClient(Vehicle);

                    delete Vehicle;
                    Vehicle = new Physics::CVehicle();
                    Physics::CServer::GetInstance()->RegisterClient(Vehicle);

                    if(Vehicle->LoadTruck(*PodPool, Config->Game.Vehicle)) {
                        Logger->Text("Loaded Truck - " + std::string(Config->Game.Vehicle));
                    } else {
                        Logger->Error("Error Loading Truck - " + std::string(Config->Game.Vehicle));
                    }

                    if(Vehicle->LoadPhysics(Config->Physics.VehicleFile)) {
                        Logger->Text("Loaded Vehicle Physics");
                    } else {
                        Logger->Error("Error Loading Vehicle Physics");
                    }

                    Vehicle->Destroy();
                    if(Vehicle->Create(Track->GetStartPosition(0), Track->GetStartRotation(0), Track->Dimensions())) {
                        Logger->Text("Created Physics Vehicle");
                    } else {
                        Logger->Error("Error Creating Physics Vehicle");
                    }

//                    Vehicle->Reset();
//                    glm::vec3 Position = Track->GetStartPosition(0);
//                    Position.y += 0.5f;
//                    Vehicle->SetPosition(Position);

                    Config->General.Paused = false;
                }

            }

            static int LastTrack = 0;
            static int ComboBox_TrackItem = 0;

            for(unsigned int i = 0; i < PodPool->NumTracks(); i++) {
                if(StrCompare(&PodPool->TrackFilename(i), Config->Game.Track)) {
                    ComboBox_TrackItem = i;
                    LastTrack = ComboBox_TrackItem;
                    continue;
                }
            }


            static char **TrackItems = new char*[PodPool->NumTracks()];

            for(unsigned int i = 0; i < PodPool->NumTracks(); i++) {
//                TrackItems[i] = &PodPool->TrackFilename(i);
                TrackItems[i] = &PodPool->TrackName(i);
            }

            ImGui::Combo("Track", &ComboBox_TrackItem, TrackItems, PodPool->NumTracks(), 10);

//            ImGui::SameLine();
//            ImGui::Text("(Restart)");

            if(ComboBox_TrackItem != LastTrack) {

                LastTrack = ComboBox_TrackItem;

//                strcpy(Config->Game.Track, TrackItems[ComboBox_TrackItem]);
                strcpy(Config->Game.Track, &PodPool->TrackFilename(ComboBox_TrackItem));

//                Loop = false;

                if (Track) {

//                    Track->Destroy();

                    delete Track;
                    Track = new Game::CTrack();
                    if (Track->Load(*PodPool, Config->Game.Track)) {
                        Logger->Text("Loaded Track - " + std::string(Config->Game.Track));
                    } else {
                        Logger->Error("Error Loading Track - " + std::string(Config->Game.Track));
                    }

                    QuadTree.Destroy();
                    if (QuadTree.Create(Track, Track->Dimensions() / 8 )) {
                        Logger->Text("Created Terrain QuadTree");
                    } else {
                        Logger->Error("Error Creating Terrain QuadTree");
                    }

                    Physics::CServer::GetInstance()->UnregisterAllClients();
                    Physics::CServer::GetInstance()->SetTerrain(Track->TerrainObject());

//                    Physics::CServer::GetInstance()->Destroy();
//                    if(Physics::CServer::GetInstance()->Create(Track->TerrainObject())) {
//                        Logger->Text("Created ODE Physics Server");
//                    } else {
//                        Logger->Error("Error Creating ODE Physics Server");
//                    }

                    Physics::CServer::GetInstance()->RegisterClient(Vehicle);

                    glm::vec3 Position = Track->GetStartPosition(0);
                    Position.y += 0.5f;
                    Vehicle->SetPosition(Position);

//                    Vehicle->SetOrientation(Track->GetStartPosition(0), Track->GetStartRotation(0));

                    Game::CModel Dummy;
                    Data.Slip = 0.000001f;

                    Objects.clear();
                    Objects.resize(Track->Models.size());
                    for (unsigned int i = 0; i < Track->Models.size(); i++) {

                        if ((Track->Models[i].Filename != "Box") && Track->GetModelTypeFlags(i) != "6,0" && Track->GetModelTypeFlags(i) != "8,0") {//&& Track->GetModelTypeFlags(i) != "8,0" ) {

                            Physics::CServer::GetInstance()->RegisterClient(&Objects[i]);
                            Objects[i].Create(dTriMeshClass, Track->GetModel(i), Track->Models[i].Mass > 0.0f ? false : true, Track->Models[i].TypeFlags[0]);
                            Objects[i].SetMass(Track->Models[i].Mass * 32.0f);
                            Objects[i].SetData(&Data);
                            Objects[i].SetRotation(Track->Models[i].Rotation.x, Track->Models[i].Rotation.y, Track->Models[i].Rotation.z);
                            Objects[i].SetPosition(Track->Models[i].Position.x, Track->Models[i].Position.y, Track->Models[i].Position.z);
                            Objects[i].SetEnviroment();

                        } else {

                            Physics::CServer::GetInstance()->RegisterClient(&Objects[i]);
                            Objects[i].Create(dBoxClass, Dummy, true, Track->Models[i].TypeFlags[0], Track->Models[i].Dimensions.y, Track->Models[i].Dimensions.x, Track->Models[i].Dimensions.z, 0, 0);
                            Objects[i].SetMass(Track->Models[i].Mass * 32.0f);
                            Objects[i].SetData(&Data);
                            Objects[i].SetRotation(Track->Models[i].Rotation.x, Track->Models[i].Rotation.y, Track->Models[i].Rotation.z);
                            Objects[i].SetPosition(Track->Models[i].Position.x, Track->Models[i].Position.y, Track->Models[i].Position.z);
                            Objects[i].SetEnviroment();

                        }
                    }

                    float Lower, Upper;
                    int Count = 0;

                    Boxes.clear();
                    for(int x = 0; x < Track->Dimensions(); x++) {
                        for(int z = 0; z < Track->Dimensions(); z++) {
                            if(Track->Boxes->GetLowerHeight(x, z) != Track->Boxes->GetUpperHeight(x, z)) {
                                Count++;
                            }
                        }
                    }

                    Boxes.resize(Count);
                    Count = 0;
                    for(int x = 0; x < Track->Dimensions(); x++) {
                        for(int z = 0; z < Track->Dimensions(); z++) {

                            Lower = Track->Boxes->GetLowerHeight(x, z);
                            Upper = Track->Boxes->GetUpperHeight(x, z);

                            if(Upper != Lower) {
                                Physics::CServer::GetInstance()->RegisterClient(&Boxes[Count]);
                                Boxes[Count].Create(dBoxClass, Dummy, true, 0, 1.0f, (Upper - Lower), 1.0f, 0.0f, 0);
                                Boxes[Count].SetMass(0);
                                Boxes[Count].SetData(&Data);
                                Boxes[Count].SetPosition(x + 0.5f, Lower + (Upper - Lower) / 2, z + 0.5f);
                                Boxes[Count].SetEnviroment();
                                Count++;
                            }
                        }
                    }

                    Weather.Create(*PodPool, Config->Game.Weather);

                    Config->General.Paused = false;

               }

            }

        }

        if(ImGui::CollapsingHeader("Help")) {

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Help"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Physics"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Game"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Render"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 0);

            ImGui::Text("Escape - Pause");
            ImGui::Text("R - Reset Truck Start");
            ImGui::Text("F - Flip Truck Upright");
            ImGui::Text("C - Cycle Camera Modes");
            ImGui::Text("Scroll Wheel - Zoom");
            ImGui::Text("Alt + Enter - Fullscreen");
            ImGui::Text("Ctrl + Q - Exit");
            ImGui::Text("Ctrl + V - Toggle VSync");
            ImGui::Text("Ctrl + P - Physics Mode");
            ImGui::Text("Ctrl + E - Editor Mode");
            ImGui::Text("Ctrl + D - Debug Info");
            ImGui::Text("Ctrl + F - Fps Counter");
            ImGui::Text("Ctrl + W - Wrap Track");
            ImGui::Text("F1 - Culling");
            ImGui::Text("F2 - Lighting");
            ImGui::Text("F3 - Texturing");
            ImGui::Text("F4 - Wireframe");
            ImGui::Text("F5 - Terrain");
            ImGui::Text("F6 - Boxes");
            ImGui::Text("F7 - Models");
            ImGui::Text("F8 - Water");
            ImGui::Text("F9 - Backdrop");
            ImGui::Text("F10 - Reflections");
            ImGui::Text("F11 - Shadows");
            ImGui::Text("W - Gas");
            ImGui::Text("S - Brake");
            ImGui::Text("A - Steer Left");
            ImGui::Text("D - Steer Right");
            ImGui::Text("Q - Down Shift");
            ImGui::Text("E - Up Shift");

        }

        if(ImGui::CollapsingHeader("Render")) {

//            ImGui::Text("Edit Mode");
//            ImGui::Separator();

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Help"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Physics"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Game"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Render"), 1);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 0);

            ImGui::Checkbox("Wrap Track     ", &Config->Render.Wrap);
            ImGui::SameLine();
            ImGui::Checkbox("Wireframe      ", &Config->Render.Wireframe);

            ImGui::Checkbox("Textured       ", &Config->Render.Textured);
            ImGui::SameLine();
            ImGui::Checkbox("Lighting       ", &Config->Render.Lighted);

            ImGui::Checkbox("Culling        ", &Config->Render.Culled);
            ImGui::SameLine();
            ImGui::Checkbox("Reflections    ", &Config->Render.Reflections);

            ImGui::Checkbox("Shadows        ", &Config->Render.Shadows);
            ImGui::SameLine();
            ImGui::Checkbox("Terrain        ", &Config->Render.Terrain);

            ImGui::Checkbox("Boxes          ", &Config->Render.Boxes);
            ImGui::SameLine();
            ImGui::Checkbox("Models         ", &Config->Render.Models);

            ImGui::Checkbox("Water          ", &Config->Render.Water);
            ImGui::SameLine();
            ImGui::Checkbox("Backdrop       ", &Config->Render.Backdrop);

        }

        if(ImGui::CollapsingHeader("Graphics")) {

            static bool Restart = false;

            float DMin, DMax;
            char UMin, UMax;

            ImGui::GetStateStorage()->SetInt(ImGui::GetID("General"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Help"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Physics"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Game"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Render"), 0);
            ImGui::GetStateStorage()->SetInt(ImGui::GetID("Graphics"), 1);

            ImGui::PushItemWidth(160);

            ImGui::Checkbox("Fullscreen      ", &Config->Graphics.Fullscreen);
            ImGui::SameLine();
            ImGui::Checkbox("Vertical Sync", &Config->Graphics.VSync);

            static int LastRes = 0;
            static int ComboBox_ResItem = 0;
            static bool LoadRes = false;

            if(LoadRes == false) {
                LoadRes = true;

                if(Config->Graphics.Width == 640 && Config->Graphics.Height == 480) {
                    ComboBox_ResItem = 0;
                }

                if(Config->Graphics.Width == 800 && Config->Graphics.Height == 600) {
                    ComboBox_ResItem = 1;
                }

                if(Config->Graphics.Width == 900 && Config->Graphics.Height == 600) {
                    ComboBox_ResItem = 2;
                }

                if(Config->Graphics.Width == 1024 && Config->Graphics.Height == 768) {
                    ComboBox_ResItem = 3;
                }

                if(Config->Graphics.Width == 1440 && Config->Graphics.Height == 900) {
                    ComboBox_ResItem = 4;
                }

                if(Config->Graphics.Width == 1600 && Config->Graphics.Height == 900) {
                    ComboBox_ResItem = 5;
                }

                if(Config->Graphics.Width == 1600 && Config->Graphics.Height == 1200) {
                    ComboBox_ResItem = 6;
                }

                if(Config->Graphics.Width == 1680 && Config->Graphics.Height == 1050) {
                    ComboBox_ResItem = 7;
                }

                if(Config->Graphics.Width == 1920 && Config->Graphics.Height == 1080) {
                    ComboBox_ResItem = 8;
                }

            }

            const char* ResItems[] = { "640x480", "800x600", "900x600", "1024x768", "1440x900", "1600x900", "1600x1200", "1680x1050", "1920x1080" };

            ImGui::Combo("Resolution", &ComboBox_ResItem, ResItems, IM_ARRAYSIZE(ResItems), 8);


            if (ComboBox_ResItem != LastRes) {

                if (ComboBox_ResItem == 0) {
                    glfwSetWindowSize(GLWindow, 640, 480);
                } else if (ComboBox_ResItem == 1) {
                    glfwSetWindowSize(GLWindow, 800, 600);
                } else if (ComboBox_ResItem == 2) {
                    glfwSetWindowSize(GLWindow, 900, 600);
                } else if (ComboBox_ResItem == 3) {
                    glfwSetWindowSize(GLWindow, 1024, 768);
                } else if (ComboBox_ResItem == 4) {
                    glfwSetWindowSize(GLWindow, 1440, 900);
                } else if (ComboBox_ResItem == 5) {
                    glfwSetWindowSize(GLWindow, 1600, 900);
                } else if (ComboBox_ResItem == 6) {
                    glfwSetWindowSize(GLWindow, 1600, 1200);
                } else if (ComboBox_ResItem == 7) {
                    glfwSetWindowSize(GLWindow, 1680, 1050);
                } else if (ComboBox_ResItem == 8) {
                    glfwSetWindowSize(GLWindow, 1920, 1080);
                }



                int Width, Height;
                glfwGetFramebufferSize(GLWindow, &Width, &Height);

                Config->Graphics.Width = Width;
                Config->Graphics.Height = Height;

                int Count, PosX, PosY;
                GLFWmonitor** Monitors = glfwGetMonitors(&Count);


                glfwSetWindowMonitor(GLWindow, NULL, 0, 0, Config->Graphics.Width, Config->Graphics.Height, Config->Graphics.RefreshRate);
                const GLFWvidmode* Mode = glfwGetVideoMode(Monitors[int(Config->Graphics.Monitor)]);
                glfwGetMonitorPos(Monitors[int(Config->Graphics.Monitor)], &PosX, &PosY);
                glfwSetWindowPos(GLWindow, PosX + (Mode->width / 2) - (Config->Graphics.Width / 2), PosY + (Mode->height / 2) - (Config->Graphics.Height / 2));


            }

            static int Count, PosX, PosY;
            static GLFWmonitor** Monitors = glfwGetMonitors(&Count);

            UMin = 0;
            UMax = Count - 1;
            ImGui::SliderScalar("Monitor", ImGuiDataType_U8, &Config->Graphics.Monitor, &UMin, &UMax, "%i");

            DMin = 16.0f;
            DMax = 256.0f;
            static float LastClip = Config->Graphics.FarClip;
            ImGui::DragScalar("Draw Distance", ImGuiDataType_Float, &Config->Graphics.FarClip, 1.0f, &DMin, &DMax, "%.1f");

            if(Config->Graphics.FarClip != LastClip) {
                LastClip = Config->Graphics.FarClip;
                Resize();
            }

            DMin = 0.0f;
            DMax = 120.0f;
            static float LastFOV = Config->Graphics.FieldOfVision;
            ImGui::DragScalar("Field Of Vision", ImGuiDataType_Float, &Config->Graphics.FieldOfVision, 1.0f, &DMin, &DMax, "%.1f");

            if(LastFOV != Config->Graphics.FieldOfVision) {
                LastFOV = Config->Graphics.FieldOfVision;
                Resize();
            }

            UMin = 0;
            UMax = 2;
            static int LastTexture = Config->Graphics.TextureQuality;
            ImGui::SliderScalar("Texture Quality", ImGuiDataType_U8, &Config->Graphics.TextureQuality, &UMin, &UMax, "%i");

            if(LastTexture != Config->Graphics.TextureQuality) {
                LastTexture = Config->Graphics.TextureQuality;
                Restart = true;
            }

            UMin = 2;
            UMax = 16;
//            float Next = pow(2, ceil(log(Config->Graphics.Antialiasing) / log(2)));
            static int LastAA = Config->Graphics.Antialiasing + 1;
            ImGui::SliderScalar("Antialiasing", ImGuiDataType_U8, &Config->Graphics.Antialiasing, &UMin, &UMax, "%i");
            if ((Config->Graphics.Antialiasing % 2) != 0) {
                Config->Graphics.Antialiasing++;
            }

            if(LastAA != Config->Graphics.Antialiasing) {
                LastAA = Config->Graphics.Antialiasing;
                Restart = true;
            }


            static bool LastMultiSample = Config->Graphics.Multisample;
            ImGui::Checkbox("Multisampling   ", &Config->Graphics.Multisample);

            if(LastMultiSample != Config->Graphics.Multisample) {
                LastMultiSample = Config->Graphics.Multisample;

                if(Config->Graphics.Multisample) {
                    glEnable(GL_MULTISAMPLE);
                } else {
                    glDisable(GL_MULTISAMPLE);
                }
            }

            ImGui::SameLine();

            static bool LastDoubleBuffer = Config->Graphics.DoubleBuffer;
            ImGui::Checkbox("Double Buffer", &Config->Graphics.DoubleBuffer);

            if(LastDoubleBuffer != Config->Graphics.DoubleBuffer) {
                LastDoubleBuffer = Config->Graphics.DoubleBuffer;
                Restart = true;
            }

            if(ImGui::Button("Apply Changes... ")) {

                if(Restart) {
                    if(MessageBox(NULL, "Changes Require A Restart, Would You Like To Continue?", "Restart", MB_ICONQUESTION | MB_YESNO) == IDYES) {
                        Loop = false;
                    }
                } else {

                    if(Config->Graphics.Multisample) {
                        glEnable(GL_MULTISAMPLE);
                    } else {
                        glDisable(GL_MULTISAMPLE);
                    }

                    std::string Resolution = std::string(ResItems[ComboBox_ResItem]);
                    Config->Graphics.Width = atoi(Resolution.substr(0, Resolution.find_first_of("x")).c_str());
                    Config->Graphics.Height = atoi(Resolution.substr(Resolution.find_last_of("x") + 1, Resolution.length()).c_str());

                    if(Config->Graphics.Fullscreen) {

                        glfwSetWindowMonitor(GLWindow, Monitors[int(Config->Graphics.Monitor)], 0, 0, Config->Graphics.Width, Config->Graphics.Height, Config->Graphics.RefreshRate);

                    } else {

                        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
                        glfwSetWindowMonitor(GLWindow, NULL, 0, 0, Config->Graphics.Width, Config->Graphics.Height, Config->Graphics.RefreshRate);

                        const GLFWvidmode* Mode = glfwGetVideoMode(Monitors[int(Config->Graphics.Monitor)]);
                        glfwGetMonitorPos(Monitors[int(Config->Graphics.Monitor)], &PosX, &PosY);
                        glfwSetWindowPos(GLWindow, PosX + (Mode->width / 2) - (Config->Graphics.Width / 2), PosY + (Mode->height / 2) - (Config->Graphics.Height / 2));

                    }

                    glfwSwapInterval(Config->Graphics.VSync);

                }

            }

//            ImGui::SameLine();
//            if (ImGui::Button("Load Defaults... ")) {
//
//                if (MessageBox(NULL, "Defaults Will Be Loaded And Restart Required, Would You Like To Continue?", "Defaults", MB_ICONEXCLAMATION | MB_YESNO) == IDYES) {
//                    Config->Defaults(ConfigFile);
//                    Loop = false;
//                }
//            }

        }

        ImGui::End();

    }

    if(Config->General.DebugMode && !Config->General.Paused) {

        ImGui::Begin("Debug Info", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize/*| ImGuiWindowFlags_AlwaysAutoResize*/ | ImGuiWindowFlags_NoCollapse);

        ImGui::SetWindowSize(ImVec2(246, 236));
        ImGui::SetWindowPos(ImVec2(Width - 250, 4));

        ImGui::Text("Gear - %i", Vehicle->GetGear());
        ImGui::Text("Speed - %0.4f", Vehicle->GetSpeed());
        ImGui::Text("Rpms - %0.4f", Vehicle->GetRpms());
        ImGui::Text("Vehicle - %0.2f, %0.2f, %0.2f", Vehicle->GetPosition()[0], Vehicle->GetPosition()[1], Vehicle->GetPosition()[2]);

        ImGui::Separator();

        switch(Config->Camera.Mode) {

        case 0:
            ImGui::Text("Camera - Cockpit");
            break;

        case 1:
            ImGui::Text("Camera - Chase");
            break;

        case 2:
            ImGui::Text("Camera - Race");
            break;

        case 3:
            ImGui::Text("Camera - Free");
            break;

        case 4:
            ImGui::Text("Camera - Orbit");
            break;

        }

        ImGui::Separator();

        ImGui::Text("Position - %0.2f, %0.2f, %0.2f", Camera.GetPosition().x, Camera.GetPosition().y, Camera.GetPosition().z);
        ImGui::Text("View - %0.2f, %0.2f, %0.2f", Camera.GetCenter().x, Camera.GetCenter().y, Camera.GetCenter().z);
        ImGui::Text("Up - %0.2f, %0.2f, %0.2f", Camera.GetUp().x, Camera.GetUp().y, Camera.GetUp().z);
        ImGui::Text("Pitch - %0.2f, Yaw - %0.2f", Camera.GetPitch(), Camera.GetYaw());
        ImGui::Text("Angle - %0.2f", Camera.GetAngle());

        if(Config->General.FpsCounter) {
            ImGui::Separator();
            ImGui::Text("Fps - %i", Timer->GetFps());
        }

        ImGui::End();
    }



    if(Config->General.PhysicsMode && !Config->General.Paused) {

        ImGui::Begin("Physics Editor", NULL, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::SetWindowSize(ImVec2(230, Height - 8), 0);
        ImGui::SetWindowPos(ImVec2(4, 4));

        ImGui::PushItemWidth(150);

        ImGui::Checkbox("Save On Exiting", &Config->Physics.Save);
        ImGui::Checkbox("Default Startup", &Config->Physics.Defaults);

//        double DMin = 0.0f, DMax = 0.0f;
//        unsigned int IMin = 0, IMax = 0;

        if(ImGui::CollapsingHeader("World        ")) {

            if(ImGui::Button("  Load  ")) {
                std::string File = FileOpen(NULL, (char*)("Physics World Files\0*.world\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "Physics\0")[0], &std::string(".world")[0]);

                if(File.size() != 0) {
                    Physics::CServer::GetInstance()->LoadPhysics(File.substr(File.find_last_of("\\") + 1));
                }
            }

            ImGui::SameLine();

            if(ImGui::Button("  Save  ")) {
                std::string File = FileSave(NULL, (char*)("Physics World Files\0*.world\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "Physics\0")[0], &std::string(".world")[0]);

                if(File.size() != 0) {
                    Physics::CServer::GetInstance()->SavePhysics(File.substr(File.find_last_of("\\") + 1));
                }
            }

            ImGui::SameLine();

            if(ImGui::Button("Defaults ")) {
                Physics::CServer::GetInstance()->Defaults("Default.world");
            }

            ImGui::Separator();

            if(ImGui::CollapsingHeader("General")) {

                ImGui::Checkbox("Quick Step", &Physics::CServer::GetInstance()->World.QuickStep);
                ImGui::Checkbox("Variable Step", &Physics::CServer::GetInstance()->World.VariableStep);

                ImGui::DragFloat("Mass Scale", &Physics::CServer::GetInstance()->World.MassScale, 0.001f, 0.000001f, 2.0f, "%.9f");
                ImGui::DragFloat("Step Size", &Physics::CServer::GetInstance()->World.StepSize, 0.001f, 0.000001f, 0.03333334f, "%.9f");
                ImGui::DragInt("Iterations", &Physics::CServer::GetInstance()->World.NumIterations, 1, 1, 120, "%i");
                ImGui::DragFloat("Gravity (Negated)", &Physics::CServer::GetInstance()->World.Gravity, 0.0001f, 0.0f, 19.74, "%.9f");
                ImGui::DragFloat("ERP", &Physics::CServer::GetInstance()->World.ERP, 0.000001f, 0.00000001f, 1.0f, "%.9f");
                ImGui::DragFloat("CFM", &Physics::CServer::GetInstance()->World.CFM, 0.000001f, 0.00000001f, 0.1f, "%.9f");
                ImGui::DragFloat("Slip", &Physics::CServer::GetInstance()->World.Slip, 0.000001f, 0.00000001f, 0.1f, "%.9f");

            }

            if(ImGui::CollapsingHeader("Collisions")) {

//                ImGui::PushItemWidth(85);

                ImGui::Text("Auto Disable Parameters");

                ImGui::Checkbox("Automatic Disable", &Physics::CServer::GetInstance()->Collisions.AutoDisableFlag);
                ImGui::DragFloat("Linear Threshold", &Physics::CServer::GetInstance()->Collisions.AutoDisableLinearThreshold, 0.000001f, 0.00000001f, 0.01f, "%.9f");
                ImGui::DragFloat("Angular Threshold", &Physics::CServer::GetInstance()->Collisions.AutoDisableAngularThreshold, 0.000001f, 0.00000001f, 0.01f, "%.9f");
                ImGui::DragInt("Steps", &Physics::CServer::GetInstance()->Collisions.AutoDisableSteps, 1, 0, 20, "%i");
                ImGui::DragFloat("Time", &Physics::CServer::GetInstance()->Collisions.AutoDisableTime, 0.000001f, 0.00000001f, 0.01f, "%.9f");

                ImGui::Text("General Parameters");

                ImGui::DragFloat("Bounce", &Physics::CServer::GetInstance()->Collisions.Bounce, 0.0001f, 0.000001f, 0.01f, "%.9f");
                ImGui::DragFloat("Bounce Velocity", &Physics::CServer::GetInstance()->Collisions.BounceVelocity, 0.0001f, 0.000001f, 0.01f, "%.9f");
                ImGui::DragFloat("Soft ERP", &Physics::CServer::GetInstance()->Collisions.SoftERP, 0.000001f, 0.00000001f, 0.01f, "%.9f");
                ImGui::DragFloat("Soft CFM", &Physics::CServer::GetInstance()->Collisions.SoftCFM, 0.000001f, 0.00000001f, 0.01f, "%.9f");

            }

//            ImGui::PushItemWidth(70);

            if(ImGui::CollapsingHeader("Damping")) {

                ImGui::DragFloat("Linear ", &Physics::CServer::GetInstance()->Damping.LinearDamping, 0.0001f, 0.000001f, 0.01f, "%.9f");
                ImGui::DragFloat("Linear Threshold ", &Physics::CServer::GetInstance()->Damping.LinearDampingThreshold, 0.0001f, 0.000001f, 0.01f, "%.9f");
                ImGui::DragFloat("Angular ", &Physics::CServer::GetInstance()->Damping.AngularDamping, 0.0001f, 0.000001f, 0.01f, "%.9f");
                ImGui::DragFloat("Angular Threshold ", &Physics::CServer::GetInstance()->Damping.AngularDampingThreshold, 0.0001f, 0.000001f, 0.01f, "%.9f");
            }

            if(ImGui::CollapsingHeader("Contacts")) {

                ImGui::DragInt("Contacts", &Physics::CServer::GetInstance()->Contacts.NumContacts, 1, 1, 60, "%i");
                ImGui::DragFloat("Correcting Velocity", &Physics::CServer::GetInstance()->Contacts.MaxCorrectingVelocity, 0.000001f, 0.00000001f, 0.01f, "%.9f");
                ImGui::DragFloat("Surface Layer", &Physics::CServer::GetInstance()->Contacts.SurfaceLayer, 0.0001f, 0.000001f, 1.0f, "%.9f");


            }

            Physics::CServer::GetInstance()->SetPhysics();
        }

        ImGui::PushItemWidth(132);

        ImGui::Separator();

        if(ImGui::CollapsingHeader("Vehicle      ")) {

            static bool AxleCommons = false;
            static bool WheelCommons = false;

            if(ImGui::Button("  Load ")) {
                std::string File = FileOpen(NULL, (char*)("Physics Vehicle Files\0*.vehicle\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "Physics\0")[0], &std::string(".vehicle")[0]);

                if(File.size() != 0) {
                    Vehicle->LoadPhysics(File.substr(File.find_last_of("\\") + 1));
                }
            }

            ImGui::SameLine();

            if(ImGui::Button("  Save   ")) {
                std::string File = FileSave(NULL, (char*)("Physics Vehicle Files\0*.vehicle\0All Files\0*.*\0"), &std::string(RootDir + ResourceDir + "Physics\0")[0], &std::string(".vehicle")[0]);

                if(File.size() != 0) {
                    Vehicle->SavePhysics(File.substr(File.find_last_of("\\") + 1));
                }
            }

            ImGui::SameLine();

            if(ImGui::Button(" Defaults")) {
                Vehicle->Defaults("Default.vehicle");
            }

            ImGui::Separator();

            if(ImGui::CollapsingHeader("General ")) {

                ImGui::InputDouble("Mass Scale ", &Vehicle->Vehicle.MassScale, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("Chassis/Body Mass", &Vehicle->Vehicle.Mass, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("Slip ", &Vehicle->Vehicle.Data.Slip, 0.000001f, 0.01f, "%.6f");

            }

            ImGui::Checkbox("Link Axle Common Attributes", &AxleCommons);

            if(AxleCommons) {

                ImGui::InputDouble("Axles Mass", &Vehicle->Vehicle.Axles[0].Mass, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("Minimum Steering", &Vehicle->Vehicle.Axles[0].MinSteer, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("Maximum Steering", &Vehicle->Vehicle.Axles[0].MaxSteer, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("Steering Speed", &Vehicle->Vehicle.Axles[0].SteerSpeed, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("Center Steer Speed", &Vehicle->Vehicle.Axles[0].CenterSpeed, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("Slip ", &Vehicle->Vehicle.Axles[0].Data.Slip, 0.000001f, 0.01f, "%.6f");

//                Vehicle->Vehicle.Axles[1] = Vehicle->Vehicle.Axles[0];

                Vehicle->Vehicle.Axles[1].Mass = Vehicle->Vehicle.Axles[0].Wheels[0].Mass;
                Vehicle->Vehicle.Axles[1].MinSteer = Vehicle->Vehicle.Axles[0].MinSteer;
                Vehicle->Vehicle.Axles[1].MaxSteer = Vehicle->Vehicle.Axles[0].MaxSteer;
                Vehicle->Vehicle.Axles[1].SteerSpeed = Vehicle->Vehicle.Axles[0].SteerSpeed;
                Vehicle->Vehicle.Axles[1].CenterSpeed = Vehicle->Vehicle.Axles[0].CenterSpeed;
                Vehicle->Vehicle.Axles[1].Data.Slip = Vehicle->Vehicle.Axles[0].Data.Slip;

            } else {

                if(ImGui::CollapsingHeader("Front Axle")) {

                    ImGui::InputDouble("Axles Mass ", &Vehicle->Vehicle.Axles[0].Mass, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Minimum Steering ", &Vehicle->Vehicle.Axles[0].MinSteer, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Maximum Steering ", &Vehicle->Vehicle.Axles[0].MaxSteer, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Steering Speed ", &Vehicle->Vehicle.Axles[0].SteerSpeed, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Center Steer Speed ", &Vehicle->Vehicle.Axles[0].CenterSpeed, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Slip  ", &Vehicle->Vehicle.Axles[0].Data.Slip, 0.000001f, 0.01f, "%.6f");

                }

                if(ImGui::CollapsingHeader("Rear Axle")) {

                    ImGui::InputDouble("Axles Mass  ", &Vehicle->Vehicle.Axles[1].Mass, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Minimum Steering  ", &Vehicle->Vehicle.Axles[1].MinSteer, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Maximum Steering  ", &Vehicle->Vehicle.Axles[1].MaxSteer, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Steering Speed  ", &Vehicle->Vehicle.Axles[1].SteerSpeed, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Center Steer Speed  ", &Vehicle->Vehicle.Axles[1].CenterSpeed, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Slip   ", &Vehicle->Vehicle.Axles[1].Data.Slip, 0.000001f, 0.01f, "%.6f");

                }
            }

            ImGui::Checkbox("Link Wheel Common Attributes", &WheelCommons);

            if(WheelCommons) {

                ImGui::InputDouble("Wheels Mass", &Vehicle->Vehicle.Axles[0].Wheels[0].Mass, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("Damping", &Vehicle->Vehicle.Axles[0].Wheels[0].Damping, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("Error", &Vehicle->Vehicle.Axles[0].Wheels[0].Error, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("Stop ERP", &Vehicle->Vehicle.Axles[0].Wheels[0].StopERP, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("Stop CFM", &Vehicle->Vehicle.Axles[0].Wheels[0].StopCFM, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("Bounce", &Vehicle->Vehicle.Axles[0].Wheels[0].Bounce, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("CFM", &Vehicle->Vehicle.Axles[0].Wheels[0].CFM, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("FMax", &Vehicle->Vehicle.Axles[0].Wheels[0].FMax, 0.000001f, 0.01f, "%.6f");
                ImGui::InputDouble("FudgeFactor", &Vehicle->Vehicle.Axles[0].Wheels[0].FudgeFactor, 0.000001f, 0.01f, "%.6f");

//                Vehicle->Vehicle.Axles[0].Wheels[1] = Vehicle->Vehicle.Axles[0].Wheels[0];
//                Vehicle->Vehicle.Axles[1].Wheels[0] = Vehicle->Vehicle.Axles[0].Wheels[0];
//                Vehicle->Vehicle.Axles[1].Wheels[1] = Vehicle->Vehicle.Axles[0].Wheels[0];

                Vehicle->Vehicle.Axles[0].Wheels[1].Mass = Vehicle->Vehicle.Axles[0].Wheels[0].Mass;
                Vehicle->Vehicle.Axles[0].Wheels[1].Damping = Vehicle->Vehicle.Axles[0].Wheels[0].Damping;
                Vehicle->Vehicle.Axles[0].Wheels[1].Error = Vehicle->Vehicle.Axles[0].Wheels[0].Error;
                Vehicle->Vehicle.Axles[0].Wheels[1].StopERP = Vehicle->Vehicle.Axles[0].Wheels[0].StopERP;
                Vehicle->Vehicle.Axles[0].Wheels[1].StopCFM = Vehicle->Vehicle.Axles[0].Wheels[0].StopCFM;
                Vehicle->Vehicle.Axles[0].Wheels[1].Bounce = Vehicle->Vehicle.Axles[0].Wheels[0].Bounce;
                Vehicle->Vehicle.Axles[0].Wheels[1].CFM = Vehicle->Vehicle.Axles[0].Wheels[0].CFM;
                Vehicle->Vehicle.Axles[0].Wheels[1].FMax = Vehicle->Vehicle.Axles[0].Wheels[0].FMax;
                Vehicle->Vehicle.Axles[0].Wheels[1].FudgeFactor = Vehicle->Vehicle.Axles[0].Wheels[0].FudgeFactor;
//                Vehicle->Vehicle.Axles[0].Wheels[1].SteerMultiplier = 0.0f;
//                Vehicle->Vehicle.Axles[0].Wheels[1].DriveMultiplier = 0.0f;

                Vehicle->Vehicle.Axles[1].Wheels[0].Mass = Vehicle->Vehicle.Axles[0].Wheels[0].Mass;
                Vehicle->Vehicle.Axles[1].Wheels[0].Damping = Vehicle->Vehicle.Axles[0].Wheels[0].Damping;
                Vehicle->Vehicle.Axles[1].Wheels[0].Error = Vehicle->Vehicle.Axles[0].Wheels[0].Error;
                Vehicle->Vehicle.Axles[1].Wheels[0].StopERP = Vehicle->Vehicle.Axles[0].Wheels[0].StopERP;
                Vehicle->Vehicle.Axles[1].Wheels[0].StopCFM = Vehicle->Vehicle.Axles[0].Wheels[0].StopCFM;
                Vehicle->Vehicle.Axles[1].Wheels[0].Bounce = Vehicle->Vehicle.Axles[0].Wheels[0].Bounce;
                Vehicle->Vehicle.Axles[1].Wheels[0].CFM = Vehicle->Vehicle.Axles[0].Wheels[0].CFM;
                Vehicle->Vehicle.Axles[1].Wheels[0].FMax = Vehicle->Vehicle.Axles[0].Wheels[0].FMax;
                Vehicle->Vehicle.Axles[1].Wheels[0].FudgeFactor = Vehicle->Vehicle.Axles[0].Wheels[0].FudgeFactor;
//                Vehicle->Vehicle.Axles[1].Wheels[0].SteerMultiplier = 0.0f;
//                Vehicle->Vehicle.Axles[1].Wheels[0].DriveMultiplier = 0.0f;

                Vehicle->Vehicle.Axles[1].Wheels[1].Mass = Vehicle->Vehicle.Axles[0].Wheels[0].Mass;
                Vehicle->Vehicle.Axles[1].Wheels[1].Damping = Vehicle->Vehicle.Axles[0].Wheels[0].Damping;
                Vehicle->Vehicle.Axles[1].Wheels[1].Error = Vehicle->Vehicle.Axles[0].Wheels[0].Error;
                Vehicle->Vehicle.Axles[1].Wheels[1].StopERP = Vehicle->Vehicle.Axles[0].Wheels[0].StopERP;
                Vehicle->Vehicle.Axles[1].Wheels[1].StopCFM = Vehicle->Vehicle.Axles[0].Wheels[0].StopCFM;
                Vehicle->Vehicle.Axles[1].Wheels[1].Bounce = Vehicle->Vehicle.Axles[0].Wheels[0].Bounce;
                Vehicle->Vehicle.Axles[1].Wheels[1].CFM = Vehicle->Vehicle.Axles[0].Wheels[0].CFM;
                Vehicle->Vehicle.Axles[1].Wheels[1].FMax = Vehicle->Vehicle.Axles[0].Wheels[0].FMax;
                Vehicle->Vehicle.Axles[1].Wheels[1].FudgeFactor = Vehicle->Vehicle.Axles[0].Wheels[0].FudgeFactor;
//                Vehicle->Vehicle.Axles[1].Wheels[1].SteerMultiplier = 0.0f;
//                Vehicle->Vehicle.Axles[1].Wheels[1].DriveMultiplier = 0.0f;


            } else {

                if(ImGui::CollapsingHeader("Front Left Wheel")) {

                    ImGui::InputDouble("Wheels Mass ", &Vehicle->Vehicle.Axles[0].Wheels[0].Mass, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Damping ", &Vehicle->Vehicle.Axles[0].Wheels[0].Damping, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Error ", &Vehicle->Vehicle.Axles[0].Wheels[0].Error, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Stop ERP ", &Vehicle->Vehicle.Axles[0].Wheels[0].StopERP, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Stop CFM ", &Vehicle->Vehicle.Axles[0].Wheels[0].StopCFM, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Bounce ", &Vehicle->Vehicle.Axles[0].Wheels[0].Bounce, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("CFM ", &Vehicle->Vehicle.Axles[0].Wheels[0].CFM, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("FMax ", &Vehicle->Vehicle.Axles[0].Wheels[0].FMax, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("FudgeFactor ", &Vehicle->Vehicle.Axles[0].Wheels[0].FudgeFactor, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Steer Multiplier ", &Vehicle->Vehicle.Axles[0].Wheels[0].SteerMultiplier, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Drive Multiplier ", &Vehicle->Vehicle.Axles[0].Wheels[0].DriveMultiplier, 0.000001f, 0.01f, "%.6f");

                }

                if(ImGui::CollapsingHeader("Front Right Wheel")) {

                    ImGui::InputDouble("Wheels Mass  ", &Vehicle->Vehicle.Axles[0].Wheels[1].Mass, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Damping  ", &Vehicle->Vehicle.Axles[0].Wheels[1].Damping, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Error  ", &Vehicle->Vehicle.Axles[0].Wheels[1].Error, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Stop ERP  ", &Vehicle->Vehicle.Axles[0].Wheels[1].StopERP, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Stop CFM  ", &Vehicle->Vehicle.Axles[0].Wheels[1].StopCFM, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Bounce  ", &Vehicle->Vehicle.Axles[0].Wheels[1].Bounce, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("CFM  ", &Vehicle->Vehicle.Axles[0].Wheels[1].CFM, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("FMax  ", &Vehicle->Vehicle.Axles[0].Wheels[1].FMax, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("FudgeFactor  ", &Vehicle->Vehicle.Axles[0].Wheels[1].FudgeFactor, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Steer Multiplier ", &Vehicle->Vehicle.Axles[0].Wheels[1].SteerMultiplier, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Drive Multiplier ", &Vehicle->Vehicle.Axles[0].Wheels[1].DriveMultiplier, 0.000001f, 0.01f, "%.6f");

                }

                if(ImGui::CollapsingHeader("Rear Left Wheel")) {

                    ImGui::InputDouble("Wheels Mass   ", &Vehicle->Vehicle.Axles[1].Wheels[0].Mass, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Damping   ", &Vehicle->Vehicle.Axles[1].Wheels[0].Damping, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Error   ", &Vehicle->Vehicle.Axles[1].Wheels[0].Error, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Stop ERP   ", &Vehicle->Vehicle.Axles[1].Wheels[0].StopERP, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Stop CFM   ", &Vehicle->Vehicle.Axles[1].Wheels[0].StopCFM, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Bounce   ", &Vehicle->Vehicle.Axles[1].Wheels[0].Bounce, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("CFM   ", &Vehicle->Vehicle.Axles[1].Wheels[0].CFM, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("FMax   ", &Vehicle->Vehicle.Axles[1].Wheels[0].FMax, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("FudgeFactor   ", &Vehicle->Vehicle.Axles[1].Wheels[0].FudgeFactor, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Steer Multiplier  ", &Vehicle->Vehicle.Axles[1].Wheels[0].SteerMultiplier, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Drive Multiplier  ", &Vehicle->Vehicle.Axles[1].Wheels[0].DriveMultiplier, 0.000001f, 0.01f, "%.6f");

                }

                if(ImGui::CollapsingHeader("Rear Right Wheel")) {

                    ImGui::InputDouble("Wheels Mass    ", &Vehicle->Vehicle.Axles[1].Wheels[1].Mass, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Damping    ", &Vehicle->Vehicle.Axles[1].Wheels[1].Damping, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Error    ", &Vehicle->Vehicle.Axles[1].Wheels[1].Error, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Stop ERP    ", &Vehicle->Vehicle.Axles[1].Wheels[1].StopERP, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Stop CFM    ", &Vehicle->Vehicle.Axles[1].Wheels[1].StopCFM, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Bounce    ", &Vehicle->Vehicle.Axles[1].Wheels[1].Bounce, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("CFM    ", &Vehicle->Vehicle.Axles[1].Wheels[1].CFM, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("FMax    ", &Vehicle->Vehicle.Axles[1].Wheels[1].FMax, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("FudgeFactor    ", &Vehicle->Vehicle.Axles[1].Wheels[1].FudgeFactor, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Steer Multiplier   ", &Vehicle->Vehicle.Axles[1].Wheels[1].SteerMultiplier, 0.000001f, 0.01f, "%.6f");
                    ImGui::InputDouble("Drive Multiplier   ", &Vehicle->Vehicle.Axles[1].Wheels[1].DriveMultiplier, 0.000001f, 0.01f, "%.6f");


                }
            }

            double Min, Max;

            if(ImGui::CollapsingHeader("Transmission")) {

                ImGui::Checkbox("Automatic", &Vehicle->Transmission.Automatic);
                ImGui::InputDouble("Horse Power", &Vehicle->Transmission.HorsePower, 1.0f, 10.0f, "%.0f");
                ImGui::InputDouble("Braking Speed", &Vehicle->Transmission.Brake, 1.0f, 10.0f, "%.0f");
                ImGui::InputDouble("Reverse Speed", &Vehicle->Transmission.ReverseSpeed, 1.0f, 10.0f, "%.0f");
                ImGui::InputDouble("Max Speed", &Vehicle->Transmission.MaxSpeed, 1.0f, 10.0f, "%.0f");
                ImGui::InputDouble("Max Rpms", &Vehicle->Transmission.MaxRpms, 1.0f, 10.0f, "%.0f");
                ImGui::InputDouble("Drag", &Vehicle->Transmission.Drag, 1.0f, 10.0f, "%.0f");

                Min = 0.0f;
                Max = 10.0f;
                ImGui::SliderScalar("Gear 0 Lower", ImGuiDataType_Double, &Vehicle->Transmission.GearAccel[0][0], &Min, &Max, "%.6f");
                ImGui::SliderScalar("Gear 1 Lower", ImGuiDataType_Double, &Vehicle->Transmission.GearAccel[1][0], &Min, &Max, "%.6f");
                ImGui::SliderScalar("Gear 2 Lower", ImGuiDataType_Double, &Vehicle->Transmission.GearAccel[2][0], &Min, &Max, "%.6f");

                ImGui::SliderScalar("Gear 0 High", ImGuiDataType_Double, &Vehicle->Transmission.GearAccel[0][1], &Min, &Max, "%.6f");
                ImGui::SliderScalar("Gear 1 High", ImGuiDataType_Double, &Vehicle->Transmission.GearAccel[1][1], &Min, &Max, "%.6f");
                ImGui::SliderScalar("Gear 2 High", ImGuiDataType_Double, &Vehicle->Transmission.GearAccel[2][1], &Min, &Max, "%.6f");

                ImGui::SliderScalar("Gear 0 Next", ImGuiDataType_Double, &Vehicle->Transmission.GearNext[0][1], &Min, &Max, "%.6f");
                ImGui::SliderScalar("Gear 1 Next", ImGuiDataType_Double, &Vehicle->Transmission.GearNext[1][1], &Min, &Max, "%.6f");
                ImGui::SliderScalar("Gear 2 Next", ImGuiDataType_Double, &Vehicle->Transmission.GearNext[2][1], &Min, &Max, "%.6f");

                ImGui::SliderScalar("Gear 0 Next", ImGuiDataType_Double, &Vehicle->Transmission.GearNext[0][2], &Min, &Max, "%.6f");
                ImGui::SliderScalar("Gear 1 Next", ImGuiDataType_Double, &Vehicle->Transmission.GearNext[1][2], &Min, &Max, "%.6f");
                ImGui::SliderScalar("Gear 2 Next", ImGuiDataType_Double, &Vehicle->Transmission.GearNext[2][2], &Min, &Max, "%.6f");


            }

            Vehicle->SetPhysics();
        }

        ImGui::End();

    }

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    glAlphaFunc(GL_GEQUAL, 0.75f);


}

void CRender::Logo(void)
{
    glCallList(OrthographicalView);

    float Size = (Config->Graphics.Width + Config->Graphics.Height) / 8;

    glTranslatef(Config->Graphics.Width / 2, Config->Graphics.Height / 5, 0.0f);

    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    LogoTexture.Bind(0);
    glBegin(GL_QUADS);

    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-Size, Size);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(Size, Size);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(Size, -Size);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-Size, -Size);

    glEnd();

}

void CRender::Water(float Delta)
{
    glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
//    glDisable(GL_LIGHTING);

    if(Config->Render.Textured) {
        Weather.ApplyWaterTint();
    }

    static float Scroll = 0.0f;
    Scroll += Delta * 0.01f;

    Track->GetWaterTexture()->Bind(0);
    glBegin(GL_QUADS);

//    glNormal3f(0.0f, -1.0f, 0.0f);

    glTexCoord2f(0.0f + Scroll, 0.0f + Scroll);
    glVertex3f(0.0f, Track->GetWaterHeight(), 0.0f);

    glTexCoord2f(0.0f + Scroll, 64.0f + Scroll);
    glVertex3f(0.0f, Track->GetWaterHeight(), Track->Dimensions());

    glTexCoord2f(64.0f + Scroll, 64.0f + Scroll);
    glVertex3f(Track->Dimensions(), Track->GetWaterHeight(), Track->Dimensions());

    glTexCoord2f(64.0f + Scroll, 0.0f + Scroll);
    glVertex3f(Track->Dimensions(), Track->GetWaterHeight(), 0.0f);

    glEnd();

    if (Config->Render.Lighted) {
        glEnable(GL_LIGHTING);
    }
    glCullFace(GL_BACK);
}

void CRender::Backdrop(bool Invert)
{

    if(Config->Game.Weather == 2 || Config->Game.Weather == 3) {
        return;
    }

    Weather.ApplyBackdropTint();

    glPushMatrix();

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CLIP_PLANE0);

    if (Invert) {

        glCullFace(GL_FRONT);
        glFrontFace(GL_CW);

        glTranslatef(Camera.GetPosition().x, Camera.GetPosition().y - (1.0f / 16.0f), Camera.GetPosition().z);

        GLdouble PlaneEqn[] = { 0.0f, -1.0f, 0.0f, 0.0f };
        glClipPlane(GL_CLIP_PLANE0, PlaneEqn);

        glScalef(1.0f, -1.0f, 1.0f);

    } else {

        glCullFace(GL_BACK);
        glFrontFace(GL_CW);

        glTranslatef(Camera.GetPosition().x, Camera.GetPosition().y - (1.0f / 16.0f), Camera.GetPosition().z);

        GLdouble PlaneEqn[] = { 0.0f, 1.0f, 0.0f, 0.0f };
        glClipPlane(GL_CLIP_PLANE0, PlaneEqn);
    }

    Weather.DrawSkyDome();
    for (int i = 0; i < Track->BackdropCount; i++) {
        Track->DrawBackground(i);
    }

    glDisable(GL_CLIP_PLANE0);
    glEnable(GL_DEPTH_TEST);

    glPopMatrix();

}

void CRender::Models(bool Invert)
{

//    glEnable(GL_DEPTH_TEST);
//    glClear(GL_STENCIL_BUFFER_BIT);
//    glClearStencil(0);

    float Dimensions = 0;
    glm::vec3 Min(0), Max(0);

    Weather.ApplyTint();

    for(int i = 0; i < Track->NumModels(); i++) {

        glm::vec3 Position = Track->GetModelPosition(i);
        glm::vec3 Rotation = Track->GetModelRotation(i);
        Dimensions = Track->GetModelBounds(i, Min, Max);

        if(Track->GetModelPriority(i) >= (int)Config->Game.Scenery) {
            continue;
        }

        if(Objects[i].Type == dTriMeshClass) {
            if(Frustum.SphereInFrustum(Objects[i].GetPosition(), Objects[i].Model->GetBoundingSphere()) > CFrustum::Outside) {

                glFrontFace(GL_CW);
//                glPushMatrix();

                Objects[i].Render();
//                glFrontFace(GL_CCW);
//
//                if (!Invert && Config->Render.Shadows) {
//                    Shadows(i);
//                }
//                glPopMatrix();

            }
//            continue;
        }

        if((Track->GetModelTypeFlags(i) != "8,0" && Track->GetModelTypeFlags(i) != "9,0") || Track->Models[i].Mass != 0) {
            continue;
        }

//        if (!Track->GetReflects(i) && Invert) {
//            continue;
//        }

        if(Frustum.SphereInFrustum(Position, Dimensions) > CFrustum::Outside) {
//        if (glm::distance(Position, Camera.GetPosition()) <  Config->Graphics.FarClip) {

            glPushMatrix();

            if(Invert) {
                glTranslatef(Position.x, Position.y, Position.z);
            } else {
                glTranslatef(Position.x, Position.y, Position.z);
            }

//            if (Track->GetModelTypeFlags(i) != "7,0" && Track->GetModelTypeFlags(i) != "8,0") {
//
////                glRotatef(270.0f - glm::degrees(Rotation.x), 0.0f, 1.0f, 0.0f);
//
//            } else {

            double Theta = 0.0f;

            if(Position.z - Camera.GetPosition().z == 0.0f) {

                if(Position.x < Camera.GetPosition().x) {
                    Theta = (glm::pi<float>());
                }

            } else {

                Theta = atan((Position.x - Camera.GetPosition().x) / (Position.z - Camera.GetPosition().z));

                if(Position.z > Camera.GetPosition().z) {
                    Theta = (glm::pi<float>()) / 2.0f - Theta;
                } else {
                    Theta = (glm::pi<float>()) * 1.5f - Theta;
                }

            }

            glRotatef(270.0f - glm::degrees(Theta), 0.0f, 1.0f, 0.0f);

//            }

            glRotatef(glm::degrees(-Rotation.z), 1.0f, 0.0f, 0.0f);
            glRotatef(glm::degrees(Rotation.y), 0.0f, 0.0f, 1.0f);

//            glDisable(GL_CLIP_PLANE0);

//            if(Invert) {
//                glCullFace(GL_FRONT);
//                glScalef(1.0f, 1.0f, 1.0f);
//            } else {
//                glCullFace(GL_BACK);
//            }

//            if(Track->Models[i].Filename != "Box") {
                glFrontFace(GL_CW);
                Track->DrawModel(i);
//                glFrontFace(GL_CCW);
//
//                if (!Invert && Config->Render.Shadows) {
//                    Shadows(i);
//                }

//            }



//            }

            glPopMatrix();
        }
    }

}

void CRender::Resize(void)
{
    if(glfwGetWindowAttrib(GLWindow, GLFW_ICONIFIED)) {
        return;
    }

    glfwGetFramebufferSize(GLWindow, &Width, &Height);

    OrthographicalView = glGenLists(1);
    glNewList(OrthographicalView, GL_COMPILE);
    glViewport(0, 0, Width, Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr(glm::ortho(0.0f, float(Config->Graphics.Width), float(Config->Graphics.Height), 0.0f)));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEndList();

    PerspectiveView = glGenLists(1);
    glNewList(PerspectiveView, GL_COMPILE);
    glViewport(0, 0, Width, Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glLoadMatrixf(glm::value_ptr(glm::perspective(glm::radians(Config->Graphics.FieldOfVision), float(Width) / float(Height), Config->Graphics.NearClip, Config->Graphics.FarClip)));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEndList();

}
