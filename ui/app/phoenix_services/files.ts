// files.ts
import api from './api';

export interface File {
  id: number;
  name: string;
  file_path: string;
  size: number;
  folder_id: number;
  user_id: string;
  created_at?: string;
  updated_at?: string;
  tokens: number;
  type: string;
  description: string;
  sharing: string;
}

export const getFilesByWorkspaceId = async (workspaceId: string): Promise<File[]> => {
  try {
    const response = await api.get<File[]>(`/file/by-workspace/${workspaceId}`);
    return response.data;
  } catch (error: any) {
    throw new Error(error.response?.data?.message || 'Failed to fetch files');
  }
};
