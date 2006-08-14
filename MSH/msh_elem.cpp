/**************************************************************************
MSHLib - Object: 
Task: 
Programing:
08/2005 WW/OK Encapsulation from rf_ele_msh
last modified
**************************************************************************/
#include "stdafx.h" // MFC
#include "mathlib.h"
// MSHLib
#include "msh_elem.h"
//========================================================================
namespace Mesh_Group
{
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
CElem::CElem(const long Index):CCore(Index)
{

   grid_adaptation = -1;
   nnodes = 0;
   nnodesHQ = 0;
   ele_dim = 1;         // Dimension of element
   patch_index = 0;
   //
   volume = 0.0;
   face_index=-1;  
   no_faces_on_surface = 0;   
   owner = NULL;
   tranform_tensor = NULL;
   angle = NULL;
   gravity_center[0] = gravity_center[1] = gravity_center[2] = 0.0;
   area = -1.0; //WW
}
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
CElem::CElem():CCore(0)
{

   grid_adaptation = -1;
   nnodes = 0;
   nnodesHQ = 0;
   ele_dim = 1;         // Dimension of element
   patch_index = 0;
   //
   volume = 0.0;
   face_index=-1;  
   no_faces_on_surface = 0;   
   owner = NULL;
   nodes.resize(8); // Nodes of face   
   tranform_tensor = NULL;
   angle = NULL;
   gravity_center[0] = gravity_center[1] = gravity_center[2] = 0.0;
   area = -1.0; //WW area = 1.0
}
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
CElem::CElem(const long Index,CElem* onwer, const int Face):CCore(Index),owner(onwer)
{
   int i, j, k, n, ne; 
   static int faceIndex_loc[10];
   static int edgeIndex_loc[10];
   no_faces_on_surface=0;
   n = owner->GetElementFaceNodes(Face, faceIndex_loc);
   face_index = Face;
   gravity_center[0] = gravity_center[1] = gravity_center[2] = 0.0;
   tranform_tensor = NULL;
   angle = NULL;
   switch(owner->geo_type)
   {
       case 1:  // 1-D bar element
           break;          
       case 2: // 2-D quadrilateral element
           nnodes = 2;
           nnodesHQ = 3;    
           ele_dim = 1;
           geo_type = 1;
           nfaces = 2;
           nedges = 0;
           break;           
       case 3: // 3-D hexahedral element 
           nnodes = 4;
           nnodesHQ = 8;      
           ele_dim = 2;
           geo_type = 2;
           nfaces = 4;
           nedges = 4;
           break;           
       case 4:  // 2-D triagular element 
           nnodes = 2;
           nnodesHQ = 3;    
           ele_dim = 1;
           geo_type = 1;
           nfaces = 2;
           nedges = 0;
           break;           
       case 5:  // 3-D tetrahedral element 
           nnodes = 3;
           nnodesHQ = 6;
           ele_dim = 2;
           geo_type = 4;
           nfaces = 3;
           nedges = 3;
           break;           
       case 6: 
           if(Face<2) 
	       {
              nnodes = 3;
              nnodesHQ = 6;
              ele_dim = 2;
              geo_type = 4;
              nfaces = 3;
              nedges = 3;
           }
           else  
	       {
              nnodes = 4;
              nnodesHQ = 8;      
              ele_dim = 2;
              geo_type = 2;
              nfaces = 4;
              nedges = 4;              
           }
           break; // 3-D prismatic element 
    }

    patch_index =  owner->patch_index;
	quadratic = owner->quadratic;
    nodes_index.resize(n);
    nodes.resize(n);

	boundary_type='B';
    for(i=0; i<n; i++)
    {
       nodes_index[i] =
       owner->nodes_index[faceIndex_loc[i]];
       nodes[i] = owner->nodes[faceIndex_loc[i]];
       nodes[i]->boundary_type = 'B';
    }
	// Face edges
    ne = owner->GetEdgesNumber();
    edges.resize(nnodes);
    edges_orientation.resize(nnodes);
    edges_orientation = 1;
    for(i=0; i<nnodes; i++)
    {
        k = (i+1)%nnodes; 
        for(j=0; j<ne; j++)
        {
           owner->GetLocalIndicesOfEdgeNodes(j, edgeIndex_loc);    
           if( (faceIndex_loc[i]==edgeIndex_loc[0]&&
                faceIndex_loc[k]==edgeIndex_loc[1])||															
                (faceIndex_loc[i]==edgeIndex_loc[1]&&
                faceIndex_loc[k]==edgeIndex_loc[0]) )
           {
               edges[i] = owner->edges[j];
               if(faceIndex_loc[i]==edgeIndex_loc[1]&&
                   faceIndex_loc[k]==edgeIndex_loc[0] )
               edges_orientation[i] = -1; 
               edges[i]->boundary_type = 'B';
               break;
           } 
       }
    } 
}


/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW/OK Implementation
**************************************************************************/
CElem::CElem( const long Index, CElem* m_ele_parent):CCore(Index)
{
  int i; 
  static int faceIndex_loc[10];
  static int edgeIndex_loc[10];
  gravity_center[0] = gravity_center[1] = gravity_center[2] = 0.0;
  tranform_tensor = NULL;
  angle = NULL;
  switch(m_ele_parent->geo_type)
  {
      case 1:
         nnodes = 2;
         nnodesHQ = 3;    
         ele_dim = 1;
         geo_type = 1;
         nfaces = 2;
         nedges = 0;
         break;
      case 2:
         nnodes = 4;
         nnodesHQ = 9;      
         ele_dim = 2;
         geo_type = 2;
         nfaces = 4;
         nedges = 4;
         break;
      case 3:
         nnodes = 8;
         nnodesHQ = 20;
         ele_dim = 3;
         nfaces = 6;
         nedges = 12;
         geo_type = 3;
         break;
      case 4:
         nnodes = 3;
         nnodesHQ = 6;
         ele_dim = 2;
         geo_type = 4;
         nfaces = 3;
         nedges = 3;
         break;
      case 5:
         nnodes = 4;
         nnodesHQ = 10;
         ele_dim = 3;
         geo_type = 5;
         nfaces = 4;
         nedges = 6;
         break;
      case 6:
         nnodes = 6;
         nnodesHQ = 15;
         ele_dim = 3;
         geo_type = 6;
         nfaces = 5;
         nedges = 9;
		 break;
    }
    patch_index =  m_ele_parent->patch_index;
	quadratic = m_ele_parent->quadratic;
    nodes_index.resize(nnodes);
    nodes.resize(nnodes);

	boundary_type='I';
    //----------------------------------------------------------------------
    // Initialize topological properties
    neighbors.resize(nfaces);
    for(i=0; i<nfaces; i++)
      neighbors[i] = NULL;
    edges.resize(nedges);    
    edges_orientation.resize(nedges);
    for(i=0; i<nedges; i++)
    {
      edges[i] = NULL;
      edges_orientation[i] = 1;
   }
   area = -1.0; //WW
}

/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
CElem::~CElem()
{
   nodes_index.resize(0);
   nodes.resize(0);
   edges.resize(0);
   neighbors.resize(0);
   mat_vector.resize(0);
   edges_orientation.resize(0);
   owner=NULL;
   if(tranform_tensor) delete tranform_tensor;
   tranform_tensor = NULL;
   if(angle) delete angle;
   angle = NULL;
   tranform_tensor = NULL;
}
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
void CElem::FillTransformMatrix()
{
   if(tranform_tensor) return;
   int i;
   double xx[3];
   double yy[3];
   double zz[3];
   tranform_tensor = new Matrix(3,3);  
   if(geo_type==1)
   {
       // x"_vec
	   xx[0] = nodes[1]->X()-nodes[0]->X();                
       xx[1] = nodes[1]->Y()-nodes[0]->Y();                  
       xx[2] = nodes[1]->Z()-nodes[0]->Z();             
       NormalizeVector(xx,3);   
       // an arbitrary vector 
       for(i=0; i<3; i++)
         yy[i] = 0.0;
	   if(fabs(xx[0])>0.0&&fabs(xx[1])+fabs(xx[2])<MKleinsteZahl)
           yy[2] = 1.0;
	   else if (fabs(xx[1])>0.0&&fabs(xx[0])+fabs(xx[2])<MKleinsteZahl)
           yy[0] = 1.0;
	   else if (fabs(xx[2])>0.0&&fabs(xx[0])+fabs(xx[1])<MKleinsteZahl)
           yy[1] = 1.0;
	   else
	   {
         for(i=0; i<3; i++)
	     {
            if(fabs(xx[i])>0.0)
		    {
                yy[i] = - xx[i];
                break;
		    }
	     }
	   }
       // z"_vec
       CrossProduction(xx,yy,zz);
       NormalizeVector(zz,3);
       // y"_vec
       CrossProduction(zz,xx,yy);
       NormalizeVector(yy,3);
   }
   else if (geo_type==2||geo_type==4)
   {
       // x"_vec
	   xx[0] = nodes[1]->X()-nodes[0]->X();                
       xx[1] = nodes[1]->Y()-nodes[0]->Y();                  
       xx[2] = nodes[1]->Z()-nodes[0]->Z();             
       NormalizeVector(xx,3);   
       // a vector on the plane
       yy[0] = nodes[2]->X()-nodes[1]->X();                  
       yy[1] = nodes[2]->Y()-nodes[1]->Y();               
       yy[2] = nodes[2]->Z()-nodes[1]->Z(); 
       // z"_vec. off plane
       CrossProduction(xx,yy,zz);
       NormalizeVector(zz,3);
       // y"_vec
       CrossProduction(zz,xx,yy);
       NormalizeVector(yy,3);
   }
   for(i=0; i<3; i++)
   {
     (*tranform_tensor)(i,0) = xx[i];          
     (*tranform_tensor)(i,1) = yy[i];         
     (*tranform_tensor)(i,2) = zz[i];        
   } 
}
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
02/2006 PCH Implementation
**************************************************************************/
double CElem::getTransformTensor(const int idx)
{
    //WW
    int i = idx%3;
    int j = idx/3;
    return (*tranform_tensor)(i,j);
	//return MatT[idx]; 
}
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
void CElem:: SetFace()
{
    nodes.resize(8);
    nodes_index.resize(8);
}
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
void CElem:: SetFace(CElem* onwer, const int Face)
{
   int i, n; 
   static int nodeIndex_loc[8];
   no_faces_on_surface=0;
   owner = onwer;
   n = owner->GetElementFaceNodes(Face, nodeIndex_loc);
   face_index = Face;
   switch(owner->geo_type)
   {
       case 1:  // 1-D bar element
           break;          
       case 2: // 2-D quadrilateral element
           break;           
       case 3: // 3-D hexahedral element 
           nnodes = 4;
           nnodesHQ = 8;      
           ele_dim = 2;
           geo_type = 2;
           nfaces = 4;
           nedges = 4;
           break;           
       case 4:  // 2-D triagular element 
           break;           
       case 5:  // 3-D tetrahedral element 
           nnodes = 3;
           nnodesHQ = 6;
           ele_dim = 2;
           geo_type = 4;
           nfaces = 3;
           nedges = 3;
           break;           
       case 6: 
           if(Face<2) 
	       {
              nnodes = 3;
              nnodesHQ = 6;
              ele_dim = 2;
              geo_type = 4;
              nfaces = 3;
              nedges = 3;
           }
           else  
	       {
              nnodes = 4;
              nnodesHQ = 8;      
              ele_dim = 2;
              geo_type = 2;
              nfaces = 4;
              nedges = 4;              
           }
           break; // 3-D prismatic element 
    }
    

	for(i=0; i<n; i++)
    {
       nodes[i] =
          owner->nodes[nodeIndex_loc[i]];
    }
}
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
string CElem::GetName() const
{
  
   switch(geo_type)
   {
     case 1:
       return "line";
       break;
     case 2:
       return "quad";
       break;
     case 3:
       return "hex";
       break;
     case 4:
       return "tri";
       break;
     case 5:
       return "tet";
       break;
     case 6:
       return "pris";
       break;
     default:
       return "none";
       break;
   }
}
/**************************************************************************
MSHLib-Method:
Programing:
06/2005 WW Implementation
08/2005 WW/OK Extension for GMS/SOL files
**************************************************************************/
void CElem::Read(istream& is, int fileType)
{ 
   //fileType=0: msh
   //fileType=1: rfi
   //fileType=2: gmsh
   //fileType=3: GMS
   //fileType=4: SOL
   int idummy, et;
   string buffer, name;
   idummy=et=-1;
//   is.ignore(numeric_limits<int>::max(), '\n');  
  //----------------------------------------------------------------------
  // 1 Reading element type data
  switch(fileType){
    //....................................................................
    case 0: // msh
      is>>index>>patch_index;
      is>>buffer;
	  if(buffer.find("-1")!=string::npos)
	  {
          grid_adaptation = strtol(buffer.data(),NULL,0);
         is>>name;
	  } 
	  else
	    name = buffer;
      if(name.find("line")!=string::npos)
         geo_type = 1;
      else if(name.find("quad")!=string::npos)
         geo_type = 2;
      else if(name.find("hex")!=string::npos)
         geo_type = 3;
      else if(name.find("tri")!=string::npos)
         geo_type = 4;
      else if(name.find("tet")!=string::npos)
         geo_type = 5;
      else if(name.find("pri")!=string::npos)
         geo_type = 6;
      break;
    //....................................................................
    case 1: // rfi
	  is>>index>>patch_index>>name;
      if(name.find("line")!=string::npos)
         geo_type = 1;
      else if(name.find("quad")!=string::npos)
         geo_type = 2;
      else if(name.find("hex")!=string::npos)
         geo_type = 3;
      else if(name.find("tri")!=string::npos)
         geo_type = 4;
      else if(name.find("tet")!=string::npos)
         geo_type = 5;
      else if(name.find("pri")!=string::npos)
         geo_type = 6;
      break;
    //....................................................................
    case 2: // gmsh
      int gmsh_patch_index; //OKCC
      is>>index>>et>>gmsh_patch_index>>idummy>>nnodes;
      patch_index = gmsh_patch_index-1; //OK
      switch(et)
      {
         case 1: geo_type = 1; break;
         case 2: geo_type = 4; break;
         case 3: geo_type = 2; break;
         case 4: geo_type = 5; break;
         case 5: geo_type = 3; break;
         case 6: geo_type = 6; break;
      }
	  index--;
      break;
    //....................................................................
    case 3: // GMS
      geo_type = 4;
      break;
    //....................................................................
    case 4: // gmsh
      geo_type = 4;
      break;
  }
  //----------------------------------------------------------------------
  // 2 Element configuration
  switch(geo_type)
   {
      case 1:
         nnodes = 2;
         nnodesHQ = 3;    
         ele_dim = 1;
         geo_type = 1;
         nfaces = 2;
         nedges = 0;
         break;
      case 2:
         nnodes = 4;
         nnodesHQ = 9;      
         ele_dim = 2;
         geo_type = 2;
         nfaces = 4;
         nedges = 4;
         break;
      case 3:
         nnodes = 8;
         nnodesHQ = 20;
         ele_dim = 3;
         nfaces = 6;
         nedges = 12;
         geo_type = 3;
         break;
      case 4:
         nnodes = 3;
         nnodesHQ = 6;
         ele_dim = 2;
         geo_type = 4;
         nfaces = 3;
         nedges = 3;
         break;
      case 5:
         nnodes = 4;
         nnodesHQ = 10;
         ele_dim = 3;
         geo_type = 5;
         nfaces = 4;
         nedges = 6;
         break;
      case 6:
         nnodes = 6;
         nnodesHQ = 15;
         ele_dim = 3;
         geo_type = 6;
         nfaces = 5;
         nedges = 9;
		 break;
   }
   nodes_index.resize(nnodes);
  //----------------------------------------------------------------------
  // 3 Reading element node data
  switch(fileType){
    //....................................................................
    case 0: // msh
      for(int i=0; i<nnodes; i++)
        is>>nodes_index[i];
      break;
    //....................................................................
    case 1: // rfi
      for(int i=0; i<nnodes; i++)
        is>>nodes_index[i];
      break;
    //....................................................................
    case 2: // gmsh
      for(int i=0; i<nnodes; i++){
        is>>nodes_index[i];
        nodes_index[i] -= 1;
      }
      break;
    //....................................................................
    case 3: // GMS
      for(int i=0; i<nnodes; i++){
        is>>nodes_index[i];
        nodes_index[i] -= 1;
      }
      break;
    //....................................................................
    case 4: // SOL
      for(int i=0; i<nnodes; i++){
        is>>nodes_index[i];
        nodes_index[i] -= 1;
      }
      is >> patch_index;
      break;
  }
  is>>ws;
  //----------------------------------------------------------------------
  // Initialize topological properties
  neighbors.resize(nfaces);
  for(int i=0; i<nfaces; i++)
    neighbors[i] = NULL;
  edges.resize(nedges);    
  edges_orientation.resize(nedges);
  for(int i=0; i<nedges; i++)
  {
    edges[i] = NULL;
	edges_orientation[i] = 1;
  }
}
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
void CElem::WriteIndex(ostream& os) const
{
    int nn;
    nn = nnodes; 
	if(quadratic) nn = nnodesHQ;
    os<<index<<deli<<patch_index<<deli<<GetName()<<deli;
    for(int i=0; i<nn; i++)
      os<<nodes_index[i]<<deli;
    os<<endl;
}
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
void CElem::WriteIndex_TEC(ostream& os) const
{
    if(geo_type==1)
       os<<nodes_index[0]+1<<deli<<nodes_index[1]+1<<deli
	     <<nodes_index[1]+1<<deli<<nodes_index[0]+1;

	else if(geo_type==4)
	{
       os<<nodes_index[0]+1<<deli<<nodes_index[1]+1<<deli
	     <<nodes_index[2]+1<<deli<<nodes_index[0]+1;   
    }
    else if(geo_type==6)
    {
       os<<nodes_index[0]+1<<deli<<nodes_index[0]+1<<deli<<nodes_index[1]+1<<deli<<nodes_index[2]+1<<deli
         <<nodes_index[3]+1<<deli<<nodes_index[3]+1<<deli<<nodes_index[4]+1<<deli<<nodes_index[5]+1<<deli;   
    }
	else
	{
         
       for(int i=0; i<nnodes; i++)
         os<<nodes_index[i]+1<<deli;
	}
    os<<endl;
}
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
void CElem::WriteAll(ostream& os) const
{
    os<<index<<deli<<patch_index<<deli<<GetName()<<deli;
    //if(index==0) 
    os<<"Index X Y Z: "<<endl;
				for(int i=0; i<nodes.Size(); i++)
    {
       os<<nodes_index[i]
       <<deli<<nodes[i]->X()
       <<deli<<nodes[i]->Y()
       <<deli<<nodes[i]->Z()<<endl;
    }

}
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
void CElem::WriteNeighbors(ostream& os) const
{
    os<<"Neighbors of "<<index<<endl;
    for(int i=0; i<nfaces; i++)
      neighbors[i]->WriteAll(os);
    os<<"End neighbors of "<<index<<endl<<endl;;
}

/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
void CElem:: MarkingAll(bool makop)
{
	int i;
	mark=makop;
    int SizeV = nnodes;
    if(quadratic) SizeV = nnodesHQ;
    for (i=0; i< SizeV;i++)
		nodes[i]->mark = mark;
    for (i=0; i< nedges;i++)
		edges[i]->mark = mark;
}
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
void CElem::SetNodes(vec<CNode*>&  ele_nodes, const bool ReSize)
{ 
    int SizeV = nnodes;
    if(quadratic) SizeV = nnodesHQ;
    if(ReSize)
    {
        nodes.resize(SizeV);
        nodes_index.resize(SizeV);
    }
    for (int i=0; i< SizeV;i++)
    {
        nodes[i] = ele_nodes[i];
        nodes_index[i] = nodes[i]->GetIndex();       
    }
}
/**************************************************************************
MSHLib-Method: 
Task:
Programing:
06/2005 WW Implementation
**************************************************************************/
void  CElem::GetLocalIndicesOfEdgeNodes(const int Edge, int *EdgeNodes)
{
	switch(geo_type)
	{
       case 1: 
           break; // 1-D bar element 
       case 2: // 2-D quadrilateral element 
          EdgeNodes[0] = Edge;
          EdgeNodes[1] = (Edge+1)%4;
          break;             
       case 3: // 3-D hexahedral element
          if(Edge<8)
          {
             EdgeNodes[0] = Edge;
             EdgeNodes[1] = (Edge+1)%4+4*(int)(Edge/4);
          }
          else 
          {
             EdgeNodes[0] = Edge%4;
             EdgeNodes[1] = Edge%4+4;
          }
          break;  
       case 4:  // 2-D triagular element 
          EdgeNodes[0] = Edge;
          EdgeNodes[1] = (Edge+1)%3;
          break;
       case 5:  // 3-D tetrahedra
          if(Edge<3)
          {
             EdgeNodes[0] = Edge;
             EdgeNodes[1] = (Edge+1)%3;
          }
          else
          {
             EdgeNodes[0] = 3;
             EdgeNodes[1] = (Edge+1)%3;
          }

          break;  
       case 6: // 3-D prismatic element
          if(Edge<6)
          {
             EdgeNodes[0] = Edge;
             EdgeNodes[1] = (Edge+1)%3+3*(int)(Edge/3);
          }
          else 
          {
             EdgeNodes[0] = Edge%3;
             EdgeNodes[1] = Edge%3+3;
          }
          break;  
	}
}
/**************************************************************************
GetElementFaceNodes
Task: Get local indeces of an element face nodes
Return: number of nodes of a face
Programing:
06/2004 WW  
**************************************************************************/
int CElem::GetElementFaces1D(int *FaceNode)
{
    FaceNode[0] = 0;
    FaceNode[1] = 1;
    return 2;
}
/**************************************************************************
GetElementFaceNodesTri
Task: Get local indeces of a traingle element face nodes
Augs.:
        const int Face :  Local index of element face 
        const int order:  1 Linear. 2, quadratic
        int *FaceNode  :  Local index of face nodes
Return: number of nodes of a face
Programing:
09/2004 WW  
**************************************************************************/
int CElem::GetElementFacesTri(const int Face, int *FaceNode)
{
	if(!quadratic)
    {
       FaceNode[0] = Face;
       FaceNode[1] = (Face+1)%3;
       return 2;
    }
    else
    {
       FaceNode[0] = Face;
       FaceNode[1] = (Face+1)%3;
       FaceNode[2] = Face+3;
       return 3;
    }
}
/**************************************************************************
GetElementFaceNodesQuad
Task: Get local indeces of a quadralateral element face nodes
Augs.:
        const int Face :  Local index of element face 
        const int order:  1 Linear. 2, quadratic
        int *FaceNode  :  Local index of face nodes
Return: number of nodes of a face
Programing:
09/2004 WW  
**************************************************************************/
int CElem::GetElementFacesQuad(const int Face, int *FaceNode)
{
    if(!quadratic)
    {
       FaceNode[0] = Face;
       FaceNode[1] = (Face+1)%4;
       return 2;
    }
    else
    {
       FaceNode[0] = Face;
       FaceNode[1] = (Face+1)%4;
       FaceNode[2] = Face+4;
       return 3;
    }
}
/**************************************************************************
GetElementFaceNodesHex
Task: Get local indeces of a hexahedra element face nodes
Augs.:
        const int Face :  Local index of element face 
        const int order:  1 Linear. 2, quadratic
        int *FaceNode  :  Local index of face nodes
Return: number of nodes of a face
Programing:
09/2004 WW  
**************************************************************************/
int CElem::GetElementFacesHex(const int Face, int *FaceNode)
{
   int nn=4, k = 0;
   if(quadratic) nn = 8;
   switch(Face)
   {
      case 0:
         for(k=0; k<4; k++)
            FaceNode[k] = k;
         if(quadratic)
         {
            for(k=0; k<4; k++)
                FaceNode[k+4] = k+8;
         }
         break;
      case 1:
         for(k=0; k<4; k++)
            FaceNode[k] = k+4;
         if(quadratic)
         {
            for(k=0; k<4; k++)
                FaceNode[k+4] = k+12;
         }
         break;
      case 2:
         FaceNode[0] = 0;
         FaceNode[1] = 4;
         FaceNode[2] = 5;
         FaceNode[3] = 1;
         if(quadratic)
         {
            FaceNode[4] = 16;
            FaceNode[5] = 12;
            FaceNode[6] = 17;
            FaceNode[7] = 8;
         }
         break;
      case 3:
         FaceNode[0] = 1;
         FaceNode[1] = 5;
         FaceNode[2] = 6;
         FaceNode[3] = 2;
         if(quadratic)
         {
            FaceNode[4] = 17;
            FaceNode[5] = 13;
            FaceNode[6] = 18;
            FaceNode[7] = 9;
         }      

         break;
      case 4:
         FaceNode[0] = 2;
         FaceNode[1] = 6;
         FaceNode[2] = 7;
         FaceNode[3] = 3;
         if(quadratic)
         {
            FaceNode[4] = 18;
            FaceNode[5] = 14;
            FaceNode[6] = 19;
            FaceNode[7] = 10;
         }
         break;
      case 5:
         FaceNode[0] = 0;
         FaceNode[1] = 3;
         FaceNode[2] = 7;
         FaceNode[3] = 4;
         if(quadratic)
         {
            FaceNode[4] = 11;
            FaceNode[5] = 19;
            FaceNode[6] = 15;
            FaceNode[7] = 16;
         }
         break;
   }
   return nn;
}
/**************************************************************************
GetElementFaceNodesTet
Task: Get local indeces of a Tedrahedra element face nodes
Augs.:
        const int Face :  Local index of element face 
        const int order:  1 Linear. 2, quadratic
        int *FaceNode  :  Local index of face nodes
Return: number of nodes of a face
Programing:
09/2004 WW  
**************************************************************************/
int CElem::GetElementFacesTet(const int Face, int *FaceNode)
{
   int nn=3;
   if(quadratic) nn =6;
   switch(Face)
   {
      case 0:
         FaceNode[0] = 1;
         FaceNode[1] = 2;
         FaceNode[2] = 3;
         if(quadratic)
         {
            FaceNode[3] = 5 ;
            FaceNode[4] = 8;
            FaceNode[5] = 7;
         }
         break;
      case 1:
         FaceNode[0] = 3;
         FaceNode[1] = 2;
         FaceNode[2] = 0;
         if(quadratic)
         {
            FaceNode[3] = 8 ;
            FaceNode[4] = 6;
            FaceNode[5] = 9;
         }
         break;
      case 2:
         FaceNode[0] = 1;
         FaceNode[1] = 3;
         FaceNode[2] = 0;
         if(quadratic)
         {
            FaceNode[3] = 7 ;
            FaceNode[4] = 9;
            FaceNode[5] = 4;
         }
         break;
      case 3:
         FaceNode[0] = 0;
         FaceNode[1] = 2;
         FaceNode[2] = 1;
         if(quadratic)
         {
            FaceNode[3] = 6 ;
            FaceNode[4] = 5;
            FaceNode[5] = 4;
         }
         break;

   }
   return nn;
}
/**************************************************************************
GetElementFaceNodesPri
Task: Get local indeces of a prismal element face nodes
Augs.:
        const int Face :  Local index of element face 
        const int order:  1 Linear. 2, quadratic
        int *FaceNode  :  Local index of face nodes
Return: number of nodes of a face
Programing:
09/2004 WW  
**************************************************************************/
int CElem::GetElementFacesPri(const int Face, int *FaceNode)
{
   int nn=3, k = 0;
   switch(Face)
   {
      case 0:
         nn = 3;
         for(k=0; k<3; k++)
            FaceNode[k] = k;
         if(quadratic)
         {
            for(k=0; k<3; k++)
                FaceNode[k+3] = k+6;
            nn = 6;
         }
         break;
      case 1:
         for(k=0; k<3; k++)
            FaceNode[k] = k+3;
         nn = 3;
         if(quadratic)
         {
            for(k=0; k<3; k++)
                FaceNode[k+3] = k+9;
            nn = 6;
         }
         break;
      case 2:
         FaceNode[0] = 1;
         FaceNode[1] = 2;
         FaceNode[2] = 5;
         FaceNode[3] = 4;
         nn = 4;
         if(quadratic)
         {
            FaceNode[4] = 7 ;
            FaceNode[5] = 14;
            FaceNode[6] = 10;
            FaceNode[7] = 13;
            nn = 8;
         }
         break;
      case 3:
         FaceNode[0] = 5;
         FaceNode[1] = 2;
         FaceNode[2] = 0;
         FaceNode[3] = 3;
         nn = 4;
         if(quadratic)
         {
            FaceNode[4] = 14 ;
            FaceNode[5] =  8;
            FaceNode[6] = 12;
            FaceNode[7] = 10;
            nn = 8;
         }
         break;
      case 4:
         FaceNode[0] = 0;
         FaceNode[1] = 1;
         FaceNode[2] = 4;
         FaceNode[3] = 3;
         nn = 4;
         if(quadratic)
         {
            FaceNode[4] = 6 ;
            FaceNode[5] = 13;
            FaceNode[6] = 9;
            FaceNode[7] = 12;
            nn = 8;
         }
         break;

   }
   return nn;
}
/**************************************************************************
GetElementFaces
Task: set element faces (Geometry)
Augs.:
        const int Face :  Local index of element face 
        const int order:  1 Linear. 2, quadratic
        int *FaceNode  :  Local index of face nodes

Programing:
09/2004 WW  
**************************************************************************/
int CElem::GetElementFaceNodes(const int Face, int *FacesNode)
{
   switch(geo_type)
   {
       case 1:  // 1-D bar element
           return GetElementFaces1D(FacesNode);
           break;          
       case 2: // 2-D quadrilateral element
           return GetElementFacesQuad(Face, FacesNode);
           break;           
       case 3: // 3-D hexahedral element 
           return GetElementFacesHex(Face, FacesNode);
           break;           
       case 4:  // 2-D triagular element 
           return GetElementFacesTri(Face, FacesNode);
           break;           
       case 5:  // 3-D tetrahedral element 
           return GetElementFacesTet(Face, FacesNode);
           break;           
       case 6: 
           return GetElementFacesPri(Face, FacesNode);
           break; // 3-D prismatic element 
    }
    return 0;
}
/**************************************************************************
FindFaceEdges(const int LocalFaceIndex, vec<CEdge*>&  face_edges)
Task: set element faces (Geometry)
Augs.:
        const int LocalFaceIndex :  Local index of element face 
        vec<CEdge*>&  face_edges :  Found face edges
retrun: number of face edges

Programing:
07/2005 WW  
**************************************************************************/
int CElem::FindFaceEdges(const int LocalFaceIndex, vec<CEdge*>&  face_edges)
{
    int i;
	i=0;
	switch(geo_type)
	{
       case 1: 
           break; // 1-D bar element 
       case 2: // 2-D quadrilateral element 
		  face_edges[0] = edges[LocalFaceIndex];
          return 1;
          break;             
       case 3: // 3-D hexahedral element
          if(LocalFaceIndex<2)
          {
             for(i=0; i<4; i++)
               face_edges[i] = edges[LocalFaceIndex*4+i];
          }
          else if(LocalFaceIndex==2) 
          {
			 face_edges[0] = edges[0];
			 face_edges[1] = edges[9];
			 face_edges[2] = edges[4];
			 face_edges[3] = edges[8];
		  }
          else if(LocalFaceIndex==3) 
          {
			 face_edges[0] = edges[1];
			 face_edges[1] = edges[9];
			 face_edges[2] = edges[5];
			 face_edges[3] = edges[10];
		  }
          else if(LocalFaceIndex==4) 
          {
			 face_edges[0] = edges[2];
			 face_edges[1] = edges[10];
			 face_edges[2] = edges[6];
			 face_edges[3] = edges[11];
		  }
          else if(LocalFaceIndex==5) 
          {
			 face_edges[0] = edges[7];
			 face_edges[1] = edges[8];
			 face_edges[2] = edges[3];
			 face_edges[3] = edges[11];
		  }

          return 4;
          break;  
       case 4:  // 2-D triagular element 
		  face_edges[0] = edges[LocalFaceIndex];
          return 1;
          break;
       case 5:  // 3-D tetrahedra
          if(LocalFaceIndex==0) 
          {
			 face_edges[0] = edges[1];
			 face_edges[1] = edges[4];
			 face_edges[2] = edges[3];
		  }
          else if(LocalFaceIndex==1) 
          {
			 face_edges[0] = edges[2];
			 face_edges[1] = edges[4];
			 face_edges[2] = edges[5];
		  }
          else if(LocalFaceIndex==2) 
          {
			 face_edges[0] = edges[0];
			 face_edges[1] = edges[3];
			 face_edges[2] = edges[5];
		  }
          else if(LocalFaceIndex==3) 
          {
			 face_edges[0] = edges[0];
			 face_edges[1] = edges[1];
			 face_edges[2] = edges[2];
		  }
          return 3;
          break;  
       case 6: // 3-D prismatic element
          if(LocalFaceIndex<2)
          {
             for(i=0; i<3; i++)
               face_edges[i] = edges[LocalFaceIndex*3+i];
             return 3;
          }
          else if(LocalFaceIndex==2) 
          {
			 face_edges[0] = edges[1];
			 face_edges[1] = edges[8];
			 face_edges[2] = edges[4];
			 face_edges[3] = edges[9];
             return 4;
		  }
          else if(LocalFaceIndex==3) 
          {
			 face_edges[0] = edges[2];
			 face_edges[1] = edges[9];
			 face_edges[2] = edges[5];
			 face_edges[3] = edges[7];
             return 4;
		  }
          else if(LocalFaceIndex==4) 
          {
			 face_edges[0] = edges[0];
			 face_edges[1] = edges[8];
			 face_edges[2] = edges[3];
			 face_edges[3] = edges[7];
             return 4;
		  }
          
          break;  
	}
	return 0;
}
/**************************************************************************
MSHLib-Method: 
Task:�For elements with straight edges and surfaces
Programing:
06/2005 WW Implementation
**************************************************************************/
void CElem::ComputeVolume()
{
    double x1buff[3];
    double x2buff[3];
    double x3buff[3];
    double x4buff[3];
    volume = 0.0;
    if(geo_type!=1)
    {
       x1buff[0] = nodes[0]->X();
       x1buff[1] = nodes[0]->Y();
       x1buff[2] = nodes[0]->Z();

       x2buff[0] = nodes[1]->X();
       x2buff[1] = nodes[1]->Y();
       x2buff[2] = nodes[1]->Z();

       x3buff[0] = nodes[2]->X();
       x3buff[1] = nodes[2]->Y();
       x3buff[2] = nodes[2]->Z();
    }

	switch(geo_type)
    {
      case 1: // Line
         x2buff[0] = nodes[nnodes-1]->X()-nodes[0]->X();
         x2buff[1] = nodes[nnodes-1]->Y()-nodes[0]->Y();
         x2buff[2] = nodes[nnodes-1]->Z()-nodes[0]->Z();
         volume = sqrt(x2buff[0]*x2buff[0]+x2buff[1]*x2buff[1]+x2buff[2]*x2buff[2]) ;
//         representative_length = sqrt(x2buff[0]*x2buff[0]+x2buff[1]*x2buff[1]+x2buff[2]*x2buff[2]) ;//CMCD
        break;
      case 4: // Triangle
         volume = ComputeDetTri(x1buff, x2buff, x3buff);
//         representative_length = sqrt(volume)*4.0;
        break;
      case 2:    // Quadralateral 
         x4buff[0] = nodes[3]->X();
         x4buff[1] = nodes[3]->Y();
         x4buff[2] = nodes[3]->Z();

         volume =  ComputeDetTri(x1buff, x2buff, x3buff)
               +ComputeDetTri(x3buff, x4buff, x1buff);
//         representative_length = sqrt(volume); 
        break;
      case 5:    // Tedrahedra 
         x4buff[0] = nodes[3]->X();
         x4buff[1] = nodes[3]->Y();
         x4buff[2] = nodes[3]->Z();

         volume =  ComputeDetTex(x1buff, x2buff, x3buff, x4buff);
//         representative_length = sqrt(volume)*6.0; 
         break;
      case 3:    // Hexehadra 
         x1buff[0] = nodes[4]->X();
         x1buff[1] = nodes[4]->Y();
         x1buff[2] = nodes[4]->Z();
      
         x2buff[0] = nodes[7]->X();
         x2buff[1] = nodes[7]->Y();
         x2buff[2] = nodes[7]->Z();

         x3buff[0] = nodes[5]->X();
         x3buff[1] = nodes[5]->Y();
         x3buff[2] = nodes[5]->Z();

         x4buff[0] = nodes[0]->X();
         x4buff[1] = nodes[0]->Y();
         x4buff[2] = nodes[0]->Z();
         volume  = ComputeDetTex(x1buff, x2buff, x3buff, x4buff);

         x1buff[0] = nodes[5]->X();
         x1buff[1] = nodes[5]->Y();
         x1buff[2] = nodes[5]->Z();
                                   
         x2buff[0] = nodes[3]->X();
         x2buff[1] = nodes[3]->Y();
         x2buff[2] = nodes[3]->Z();
                                   
         x3buff[0] = nodes[1]->X();
         x3buff[1] = nodes[1]->Y();
         x3buff[2] = nodes[1]->Z();
                                   
         x4buff[0] = nodes[0]->X();
         x4buff[1] = nodes[0]->Y();
         x4buff[2] = nodes[0]->Z();
         volume  += ComputeDetTex(x1buff, x2buff, x3buff, x4buff);

         x1buff[0] = nodes[5]->X();
         x1buff[1] = nodes[5]->Y();
         x1buff[2] = nodes[5]->Z();
                                   
         x2buff[0] = nodes[7]->X();
         x2buff[1] = nodes[7]->Y();
         x2buff[2] = nodes[7]->Z();
                                   
         x3buff[0] = nodes[3]->X();
         x3buff[1] = nodes[3]->Y();
         x3buff[2] = nodes[3]->Z();
                                   
         x4buff[0] = nodes[0]->X();
         x4buff[1] = nodes[0]->Y();
         x4buff[2] = nodes[0]->Z();
         volume  += ComputeDetTex(x1buff, x2buff, x3buff, x4buff);

         x1buff[0] = nodes[5]->X();
         x1buff[1] = nodes[5]->Y();
         x1buff[2] = nodes[5]->Z();
                                   
         x2buff[0] = nodes[7]->X();
         x2buff[1] = nodes[7]->Y();
         x2buff[2] = nodes[7]->Z();
                                   
         x3buff[0] = nodes[6]->X();
         x3buff[1] = nodes[6]->Y();
         x3buff[2] = nodes[6]->Z();
                                   
         x4buff[0] = nodes[2]->X();
         x4buff[1] = nodes[2]->Y();
         x4buff[2] = nodes[2]->Z();
         volume  += ComputeDetTex(x1buff, x2buff, x3buff, x4buff);

         x1buff[0] = nodes[1]->X();
         x1buff[1] = nodes[1]->Y();
         x1buff[2] = nodes[1]->Z();
                                   
         x2buff[0] = nodes[3]->X();
         x2buff[1] = nodes[3]->Y();
         x2buff[2] = nodes[3]->Z();
                                   
         x3buff[0] = nodes[5]->X();
         x3buff[1] = nodes[5]->Y();
         x3buff[2] = nodes[5]->Z();
                                   
         x4buff[0] = nodes[2]->X();
         x4buff[1] = nodes[2]->Y();
         x4buff[2] = nodes[2]->Z();
         volume  += ComputeDetTex(x1buff, x2buff, x3buff, x4buff);

         x1buff[0] = nodes[3]->X();
         x1buff[1] = nodes[3]->Y();
         x1buff[2] = nodes[3]->Z();
                                   
         x2buff[0] = nodes[7]->X();
         x2buff[1] = nodes[7]->Y();
         x2buff[2] = nodes[7]->Z();
                                   
         x3buff[0] = nodes[5]->X();
         x3buff[1] = nodes[5]->Y();
         x3buff[2] = nodes[5]->Z();
                                   
         x4buff[0] = nodes[2]->X();
         x4buff[1] = nodes[2]->Y();
         x4buff[2] = nodes[2]->Z();

         volume  += ComputeDetTex(x1buff, x2buff, x3buff, x4buff);
//         representative_length = pow(volume,1./3.);
         break;
      case 6:    // Prism
         x4buff[0] = nodes[3]->X();
         x4buff[1] = nodes[3]->Y();
         x4buff[2] = nodes[3]->Z();
         volume =  ComputeDetTex(x1buff, x2buff, x3buff, x4buff);
       
         x1buff[0] = nodes[1]->X();
         x1buff[1] = nodes[1]->Y();
         x1buff[2] = nodes[1]->Z();
                                   
         x2buff[0] = nodes[4]->X();
         x2buff[1] = nodes[4]->Y();
         x2buff[2] = nodes[4]->Z();
                                   
         x3buff[0] = nodes[2]->X();
         x3buff[1] = nodes[2]->Y();
         x3buff[2] = nodes[2]->Z();
                                   
         x4buff[0] = nodes[3]->X();
         x4buff[1] = nodes[3]->Y();
         x4buff[2] = nodes[3]->Z();
         volume  += ComputeDetTex(x1buff, x2buff, x3buff, x4buff);

         x1buff[0] = nodes[2]->X();
         x1buff[1] = nodes[2]->Y();
         x1buff[2] = nodes[2]->Z();
                                   
         x2buff[0] = nodes[4]->X();
         x2buff[1] = nodes[4]->Y();
         x2buff[2] = nodes[4]->Z();
                                   
         x3buff[0] = nodes[5]->X();
         x3buff[1] = nodes[5]->Y();
         x3buff[2] = nodes[5]->Z();
                                   
         x4buff[0] = nodes[3]->X();
         x4buff[1] = nodes[3]->Y();
         x4buff[2] = nodes[3]->Z();
         volume  += ComputeDetTex(x1buff, x2buff, x3buff, x4buff);
//         representative_length = pow(volume,1./3.);//Here the direction of flow needs to be taken into account, we need rep length in x,y,z direction
         break;
    }
}

/**************************************************************************
MSHLib-Method:
Programing:
10/2005 OK Implementation
**************************************************************************/
void CElem::Config()
{ 
  switch(geo_type)
   {
      case 1:
         nnodes = 2;
         nnodesHQ = 3;    
         ele_dim = 1;
         geo_type = 1;
         nfaces = 2;
         nedges = 0;
         break;
      case 2:
         nnodes = 4;
         nnodesHQ = 9;      
         ele_dim = 2;
         geo_type = 2;
         nfaces = 4;
         nedges = 4;
         break;
      case 3:
         nnodes = 8;
         nnodesHQ = 20;
         ele_dim = 3;
         nfaces = 6;
         nedges = 12;
         geo_type = 3;
         break;
      case 4:
         nnodes = 3;
         nnodesHQ = 6;
         ele_dim = 2;
         geo_type = 4;
         nfaces = 3;
         nedges = 3;
         break;
      case 5:
         nnodes = 4;
         nnodesHQ = 10;
         ele_dim = 3;
         geo_type = 5;
         nfaces = 4;
         nedges = 6;
         break;
      case 6:
         nnodes = 6;
         nnodesHQ = 15;
         ele_dim = 3;
         geo_type = 6;
         nfaces = 5;
         nedges = 9;
		 break;
   }
}

/**************************************************************************
MSHLib-Method: 
Task:
Programing:
01/2006 YD Implementation
**************************************************************************/
void CElem::FaceNormal(int index0, int index1, double* face)
{
   int i;
   double xx[3];
   double yy[3];
   double zz[3];
   if (GetElementType() == 4 || GetElementType() == 2){
//----plane normal----------------------------
      // tranform_tensor = new Matrix(3,3); 
       // face"_vec
	   face[0] = nodes[index1]->X()-nodes[index0]->X();                
       face[1] = nodes[index1]->Y()-nodes[index0]->Y();                  
       face[2] = nodes[index1]->Z()-nodes[index0]->Z();  
       // x"_vec
	   xx[0] = nodes[1]->X()-nodes[0]->X();                
       xx[1] = nodes[1]->Y()-nodes[0]->Y();                  
       xx[2] = nodes[1]->Z()-nodes[0]->Z();             
       //NormalizeVector(xx,3);   
       // a vector on the plane
       yy[0] = nodes[2]->X()-nodes[1]->X();                  
       yy[1] = nodes[2]->Y()-nodes[1]->Y();               
       yy[2] = nodes[2]->Z()-nodes[1]->Z(); 
       // z"_vec
       CrossProduction(xx,yy,zz);
       NormalizeVector(zz,3);
       // y"_vec
       CrossProduction(face,zz,yy);
       NormalizeVector(yy,3);
       for(i=0; i<3; i++)
	   {
         face[i] = yy[i];         
	   } 
   }
}

//--------------------------------
} // namespace Mesh_Group
//========================================================================
