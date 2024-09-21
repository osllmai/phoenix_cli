// folders.ts
import api from './api';

export interface Folder {
  id: number;
  name: string;
  workspace_id: number;
  user_id: string;
  parent_folder_id?: number;
  created_at?: string;
  updated_at?: string;
  // Add other folder fields as needed
}

export const getFoldersByWorkspaceId = async (workspaceId: string): Promise<Folder[]> => {
  try {
    const response = await api.get<Folder[]>(`/folder/by-workspace/${workspaceId}`);
    return response.data;
  } catch (error: any) {
    throw new Error(error.response?.data?.message || 'Failed to fetch folders');
  }
};
