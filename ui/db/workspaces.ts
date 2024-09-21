import {supabase} from "@/lib/supabase/browser-client"
import {TablesInsert, TablesUpdate} from "@/supabase/types"

export const getHomeWorkspaceByUserId = async (userId: string) => {
  const { data: homeWorkspace, error } = await supabase
    .from("workspaces")
    .select("*")
    .eq("user_id", userId)
    .eq("is_home", true)
    .single()

  if (!homeWorkspace) {
    throw new Error(error.message)
  }

  return homeWorkspace.id
}

export const getWorkspaceById = async (workspaceId: string) => {
  const token = localStorage.getItem('access_token');

  if (!token) {
    throw new Error('No access token found');
  }

  const response = await fetch(`http://localhost:18080/workspace/${workspaceId}`, {
    method: 'GET',
    headers: {
      'Authorization': token,
      'Content-Type': 'application/json'
    }
  });

  if (!response.ok) {
    const errorData = await response.json();
    throw new Error(errorData.message || 'Failed to fetch workspace');
  }

  return await response.json();
};


export const getWorkspacesByUserId = async (userId: string) => {
  const token = localStorage.getItem('access_token');

  if (!token) {
    throw new Error('No access token found');
  }

  const response = await fetch(`http://localhost:18080/workspace`, {
    method: 'GET',
    headers: {
      'Authorization': token,
      'Content-Type': 'application/json'
    }
  });

  if (!response.ok) {
    const errorData = await response.json();
    throw new Error(errorData.message || 'Failed to fetch workspace');
  }

  return await response.json();
}

export const createWorkspace = async (
  workspace: TablesInsert<"workspaces">
) => {
  const { data: createdWorkspace, error } = await supabase
    .from("workspaces")
    .insert([workspace])
    .select("*")
    .single()

  if (error) {
    throw new Error(error.message)
  }

  return createdWorkspace
}

export const updateWorkspace = async (
  workspaceId: string,
  workspace: TablesUpdate<"workspaces">
) => {
  const { data: updatedWorkspace, error } = await supabase
    .from("workspaces")
    .update(workspace)
    .eq("id", workspaceId)
    .select("*")
    .single()

  if (error) {
    throw new Error(error.message)
  }

  return updatedWorkspace
}

export const deleteWorkspace = async (workspaceId: string) => {
  const { error } = await supabase
    .from("workspaces")
    .delete()
    .eq("id", workspaceId)

  if (error) {
    throw new Error(error.message)
  }

  return true
}
