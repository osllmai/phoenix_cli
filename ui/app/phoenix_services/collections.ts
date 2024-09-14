// collections.ts
import api from './api';

export interface Collection {
  id: number;
  name: string;
  description?: string;
  workspace_id: number;
  user_id: string;
  created_at?: string;
  updated_at?: string;
  // Add other collection fields as needed
}

export const getCollectionsByWorkspaceId = async (workspaceId: string): Promise<Collection[]> => {
  try {
    const response = await api.get<Collection[]>(`/collection/by-workspace/${workspaceId}`);
    return response.data;
  } catch (error: any) {
    throw new Error(error.response?.data?.message || 'Failed to fetch collections');
  }
};
