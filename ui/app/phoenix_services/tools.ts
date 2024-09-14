// tools.ts
import api from './api';

export interface Tool {
  id: number;
  name: string;
  description?: string;
  type: string;
  workspace_id: number;
  user_id: string;
  created_at?: string;
  updated_at?: string;
  // Add other tool fields as needed
}

export const getToolsByWorkspaceId = async (workspaceId: string): Promise<Tool[]> => {
  try {
    const response = await api.get<Tool[]>(`/tool/by-workspace/${workspaceId}`);
    return response.data;
  } catch (error: any) {
    throw new Error(error.response?.data?.message || 'Failed to fetch tools');
  }
};
