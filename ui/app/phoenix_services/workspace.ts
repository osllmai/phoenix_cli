// workspace.ts
import api from './api';

export interface Workspace {
  created_at: string;
  default_context_length: number;
  default_model?: string;
  default_prompt?: string;
  default_temperature?: number;
  description?: string;
  embeddings_provider?: "openai" | "local";
  id: string;
  include_profile_context: boolean;
  include_workspace_instructions: boolean;
  instructions?: string;
  is_home: boolean;
  name: string;
  updated_at: string;
  user_id: string;

}

export const getWorkspaceById = async (workspaceId: string): Promise<Workspace> => {
  try {
    const response = await api.get<Workspace>(`/workspace/${workspaceId}`);
    return response.data;
  } catch (error: any) {
    throw new Error(error.response?.data?.message || 'Failed to fetch workspace');
  }
};
