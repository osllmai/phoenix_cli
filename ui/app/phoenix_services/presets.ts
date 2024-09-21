// presets.ts
import api from './api';

export interface Preset {
  id: number;
  name: string;
  settings: Record<string, any>;
  workspace_id: number;
  user_id: string;
  created_at?: string;
  updated_at?: string;
  // Add other preset fields as needed
}

export const getPresetsByWorkspaceId = async (workspaceId: string): Promise<Preset[]> => {
  try {
    const response = await api.get<Preset[]>(`/preset/by-workspace/${workspaceId}`);
    return response.data;
  } catch (error: any) {
    throw new Error(error.response?.data?.message || 'Failed to fetch presets');
  }
};
