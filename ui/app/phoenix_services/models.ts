// models.ts
import api from './api';

export interface Model {
  id: number;
  name: string;
  type: string;
  workspace_id: number;
  user_id: string;
  created_at?: string;
  updated_at?: string;
  // Add other model fields as needed
}

export const getModelsByWorkspaceId = async (workspaceId: string): Promise<Model[]> => {
  try {
    const response = await api.get<Model[]>(`/model/by-workspace/${workspaceId}`);
    return response.data;
  } catch (error: any) {
    throw new Error(error.response?.data?.message || 'Failed to fetch models');
  }
};
