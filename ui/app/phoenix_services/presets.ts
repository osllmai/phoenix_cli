// presets.ts
import api from './api';

export interface Preset {
  id: number;
  name: string;
  folder_id: number;
  user_id: string;
  created_at?: string;
  updated_at?: string;
  sharing: string;
  context_length: number;
  description: string;
  embedding_provider: string;
  include_profile_context: boolean;
  include_workspace_instructions: boolean;
  model: string;
  prompt: string;
  temperature: number;
}

export const getPresetsByWorkspaceId = async (workspaceId: string): Promise<Preset[]> => {
  try {
    const response = await api.get<Preset[]>(`/preset/by-workspace/${workspaceId}`);
    return response.data;
  } catch (error: any) {
    throw new Error(error.response?.data?.message || 'Failed to fetch presets');
  }
};
